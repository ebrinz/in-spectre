#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <heltec.h>
#include <AS726X.h>

// ===== COMMUNICATION MODE SELECTION =====
// Set to true to use Serial-over-USB, false to use MQTT over WiFi
// Default is Serial for easier setup
#define USE_SERIAL_MODE true

// Wi-Fi and MQTT settings - update these to match your network
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_SERVER_IP";
const int mqtt_port = 1883;

// MQTT topics
const char* topic_spectral = "sensor/spectral";

// Update interval (milliseconds)
const unsigned long updateInterval = 5000;
unsigned long lastUpdateTime = 0;

// Serial communication settings
const unsigned long serialBaudRate = 115200;

// Create sensor object
AS726X spectralSensor;

// Create WiFi and MQTT clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// I2C pins for Heltec WiFi LoRa 32 V3
#define SDA_PIN 17
#define SCL_PIN 18

// Status flags
bool sensor_available = false;
bool mqtt_connected = false;
bool wifi_connected = false;
uint8_t sensorType = 0;

void setup() {
  // Initialize Heltec board (display, serial, etc.)
  Heltec.begin(true /*DisplayEnable*/, false /*LoRaEnable*/, true /*SerialEnable*/, false /*PABOOST*/, 0 /*BAND*/);
  
  // Set serial baud rate (for USB communication)
  Serial.begin(serialBaudRate);
  Serial.println("\n\n--- In-Spectre Sensor System ---");
  
  // Display setup message
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "In-Spectre");
  Heltec.display->drawString(0, 10, "Initializing...");
  Heltec.display->display();
  
  // Set up I2C with the correct pins for Heltec V3
  // Give more time for display and I2C to initialize
  delay(100);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);
  
  // Initialize sensor
  Heltec.display->drawString(0, 20, "Initializing sensor...");
  Heltec.display->display();
  
  // Try to initialize spectral sensor
  sensor_available = spectralSensor.begin(Wire);
  
  // Check sensor type
  if (sensor_available) {
    sensorType = spectralSensor.getVersion();
    String sensorName = "Unknown";
    
    if (sensorType == SENSORTYPE_AS7261) sensorName = "AS7261 (XYZ)";
    else if (sensorType == SENSORTYPE_AS7262) sensorName = "AS7262 (VIS)";
    else if (sensorType == SENSORTYPE_AS7263) sensorName = "AS7263 (NIR)";
    
    Serial.print("Spectral sensor type: 0x");
    Serial.print(sensorType, HEX);
    Serial.print(" - ");
    Serial.println(sensorName);
    
    Heltec.display->drawString(0, 30, "Sensor: " + sensorName);
  }
  
  // Display sensor status
  Heltec.display->drawString(0, 40, "Status: " + String(sensor_available ? "OK" : "FAIL"));
  Heltec.display->display();
  delay(1000);
  
  // Setup communication based on mode
  if (!USE_SERIAL_MODE) {
    // Connect to WiFi and MQTT
    setupWifi();
    mqttClient.setServer(mqtt_server, mqtt_port);
  } else {
    Serial.println("Using Serial-over-USB mode. No WiFi/MQTT connections required.");
    Heltec.display->drawString(0, 50, "Mode: USB Serial");
    Heltec.display->display();
    delay(1000);
  }
  
  // Final setup message
  updateDisplay();
}

void loop() {
  // MQTT connection handling (if in MQTT mode)
  if (!USE_SERIAL_MODE) {
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();
  }
  
  // Update sensor readings at the specified interval
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentMillis;
    
    // Read sensor data and publish based on mode
    if (sensor_available) {
      if (USE_SERIAL_MODE) {
        readAndSerializeSensor();
      } else {
        readAndPublishSensor();
      }
    }
    
    // Update display
    updateDisplay();
  }
}

void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Connecting to WiFi");
  Heltec.display->drawString(0, 10, ssid);
  Heltec.display->display();

  WiFi.begin(ssid, password);

  // Wait for connection with timeout
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    Heltec.display->drawString(timeout, 20, ".");
    Heltec.display->display();
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "WiFi Connected");
    Heltec.display->drawString(0, 10, WiFi.localIP().toString());
    Heltec.display->display();
  } else {
    wifi_connected = false;
    Serial.println("WiFi connection failed\!");
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "WiFi Failed\!");
    Heltec.display->drawString(0, 10, "Check credentials");
    Heltec.display->display();
  }
  delay(1000);
}

void reconnectMQTT() {
  int attempts = 0;
  
  // Try to connect 3 times, then continue (we'll try again next loop)
  while (!mqttClient.connected() && attempts < 3) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      mqtt_connected = true;
      Serial.println("connected");
    } else {
      mqtt_connected = false;
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
      attempts++;
    }
  }
}

// Spectral Sensor Functions - MQTT Mode
void readAndPublishSensor() {
  // Take a measurement
  spectralSensor.takeMeasurements();
  
  // Create a JSON document for the sensor data
  StaticJsonDocument<512> doc;
  doc["sensor_type"] = sensorType;
  doc["timestamp"] = millis() / 1000.0;
  doc["temperature"] = spectralSensor.getTemperatureF();
  
  // Add wavelength data based on detected sensor type
  if (sensorType == SENSORTYPE_AS7262) {
    // AS7262 (Visible)
    doc["v_450nm"] = spectralSensor.getCalibratedViolet();
    doc["b_500nm"] = spectralSensor.getCalibratedBlue();
    doc["g_550nm"] = spectralSensor.getCalibratedGreen();
    doc["y_570nm"] = spectralSensor.getCalibratedYellow();
    doc["o_600nm"] = spectralSensor.getCalibratedOrange();
    doc["r_650nm"] = spectralSensor.getCalibratedRed();
  } 
  else if (sensorType == SENSORTYPE_AS7263) {
    // AS7263 (NIR)
    doc["r_610nm"] = spectralSensor.getCalibratedR();
    doc["s_680nm"] = spectralSensor.getCalibratedS();
    doc["t_730nm"] = spectralSensor.getCalibratedT();
    doc["u_760nm"] = spectralSensor.getCalibratedU();
    doc["v_810nm"] = spectralSensor.getCalibratedV();
    doc["w_860nm"] = spectralSensor.getCalibratedW();
  }
  else if (sensorType == SENSORTYPE_AS7261) {
    // AS7261 (XYZ color sensor)
    doc["x"] = spectralSensor.getCalibratedX(); 
    doc["y"] = spectralSensor.getCalibratedY();
    doc["z"] = spectralSensor.getCalibratedZ();
    doc["nir"] = spectralSensor.getNir();
    doc["dark"] = spectralSensor.getDark();
    doc["clear"] = spectralSensor.getClear();
  }
  
  // Publish to MQTT
  char buffer[512];
  serializeJson(doc, buffer);
  mqttClient.publish(topic_spectral, buffer);
  
  Serial.println("Published spectral data to MQTT");
}

// Spectral Sensor Functions - Serial Mode
void readAndSerializeSensor() {
  // Take a measurement
  spectralSensor.takeMeasurements();
  
  // Create a JSON document for the sensor data
  StaticJsonDocument<512> doc;
  doc["sensor_type"] = sensorType;
  doc["timestamp"] = millis() / 1000.0;
  doc["temperature"] = spectralSensor.getTemperatureF();
  
  // Add human-readable sensor type name
  if (sensorType == SENSORTYPE_AS7261) {
    doc["sensor_name"] = "AS7261 (XYZ)";
  } else if (sensorType == SENSORTYPE_AS7262) {
    doc["sensor_name"] = "AS7262 (VIS)";
  } else if (sensorType == SENSORTYPE_AS7263) {
    doc["sensor_name"] = "AS7263 (NIR)";
  } else {
    doc["sensor_name"] = "Unknown";
  }
  
  // Add wavelength data based on detected sensor type
  if (sensorType == SENSORTYPE_AS7262) {
    // AS7262 (Visible)
    doc["v_450nm"] = spectralSensor.getCalibratedViolet();
    doc["b_500nm"] = spectralSensor.getCalibratedBlue();
    doc["g_550nm"] = spectralSensor.getCalibratedGreen();
    doc["y_570nm"] = spectralSensor.getCalibratedYellow();
    doc["o_600nm"] = spectralSensor.getCalibratedOrange();
    doc["r_650nm"] = spectralSensor.getCalibratedRed();
  } 
  else if (sensorType == SENSORTYPE_AS7263) {
    // AS7263 (NIR)
    doc["r_610nm"] = spectralSensor.getCalibratedR();
    doc["s_680nm"] = spectralSensor.getCalibratedS();
    doc["t_730nm"] = spectralSensor.getCalibratedT();
    doc["u_760nm"] = spectralSensor.getCalibratedU();
    doc["v_810nm"] = spectralSensor.getCalibratedV();
    doc["w_860nm"] = spectralSensor.getCalibratedW();
  }
  else if (sensorType == SENSORTYPE_AS7261) {
    // AS7261 (XYZ color sensor)
    doc["x"] = spectralSensor.getCalibratedX(); 
    doc["y"] = spectralSensor.getCalibratedY();
    doc["z"] = spectralSensor.getCalibratedZ();
    doc["nir"] = spectralSensor.getNir();
    doc["dark"] = spectralSensor.getDark();
    doc["clear"] = spectralSensor.getClear();
  }
  
  // Send over serial
  serializeJson(doc, Serial);
  Serial.println(); // Add newline for readability
  
  Serial.println("Sent spectral data over Serial");
}

// Display functions
void updateDisplay() {
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "In-Spectre Sensor");
  
  // Sensor status and type
  String sensorName = "Unknown";
  if (sensorType == SENSORTYPE_AS7261) sensorName = "AS7261 (XYZ)";
  else if (sensorType == SENSORTYPE_AS7262) sensorName = "AS7262 (VIS)";
  else if (sensorType == SENSORTYPE_AS7263) sensorName = "AS7263 (NIR)";
  
  Heltec.display->drawString(0, 12, "Sensor: " + (sensor_available ? sensorName : "Not Found"));
  Heltec.display->drawString(0, 24, "Temp: " + String(sensor_available ? spectralSensor.getTemperatureF() : 0) + "°F");
  
  // Communication mode and status
  if (USE_SERIAL_MODE) {
    Heltec.display->drawString(0, 36, "Mode: USB Serial");
    Heltec.display->drawString(0, 48, "Baud: " + String(serialBaudRate));
  } else {
    // Connectivity status
    Heltec.display->drawString(0, 36, "WiFi: " + String(wifi_connected ? "Connected" : "Disconnected"));
    Heltec.display->drawString(0, 48, "MQTT: " + String(mqtt_connected ? "Connected" : "Disconnected"));
  }
  
  Heltec.display->display();
}
