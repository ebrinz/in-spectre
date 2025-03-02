// In-Spectre with direct display control
// This version bypasses the Heltec library and uses SSD1306 directly

#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AS726X.h>

// ===== COMMUNICATION MODE SELECTION =====
// Set to true to use Serial-over-USB, false to use MQTT over WiFi
// Default is Serial for easier setup
#define USE_SERIAL_MODE true

// Display settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing reset pin)
#define SCREEN_ADDRESS 0x3C // I2C address of display

// I2C pins for Heltec ESP32
// If display_test found Option A working, use these:
#define SDA_PIN 4
#define SCL_PIN 15
// If display_test found Option B or C working, replace with these:
// #define SDA_PIN 21
// #define SCL_PIN 22

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

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create WiFi and MQTT clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Status flags
bool sensor_available = false;
bool display_available = false;
bool mqtt_connected = false;
bool wifi_connected = false;
uint8_t sensorType = 0;

void setup() {
  // Start serial communication
  Serial.begin(serialBaudRate);
  delay(500);
  Serial.println("\n\n--- In-Spectre Sensor System ---");
  
  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);
  
  // Try to initialize the display
  display_available = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  
  if (display_available) {
    Serial.println("Display initialized successfully\!");
    // Clear the buffer
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("In-Spectre");
    display.setCursor(0, 10);
    display.println("Initializing...");
    display.display();
  } else {
    Serial.println("Display initialization failed\!");
  }
  
  // Initialize sensor
  if (display_available) {
    display.setCursor(0, 20);
    display.println("Initializing sensor...");
    display.display();
  }
  
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
    
    if (display_available) {
      display.setCursor(0, 30);
      display.println("Sensor: " + sensorName);
      display.display();
    }
  }
  
  // Display sensor status
  if (display_available) {
    display.setCursor(0, 40);
    display.println("Status: " + String(sensor_available ? "OK" : "FAIL"));
    display.display();
  }
  delay(1000);
  
  // Setup communication based on mode
  if (\!USE_SERIAL_MODE) {
    // Connect to WiFi and MQTT
    setupWifi();
    mqttClient.setServer(mqtt_server, mqtt_port);
  } else {
    Serial.println("Using Serial-over-USB mode. No WiFi/MQTT connections required.");
    if (display_available) {
      display.setCursor(0, 50);
      display.println("Mode: USB Serial");
      display.display();
    }
    delay(1000);
  }
  
  // Final setup message
  updateDisplay();
}

void loop() {
  // MQTT connection handling (if in MQTT mode)
  if (\!USE_SERIAL_MODE) {
    if (\!mqttClient.connected()) {
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
  
  if (display_available) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connecting to WiFi");
    display.setCursor(0, 10);
    display.println(ssid);
    display.display();
  }

  WiFi.begin(ssid, password);

  // Wait for connection with timeout
  int timeout = 0;
  while (WiFi.status() \!= WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    if (display_available) {
      display.setCursor(timeout, 20);
      display.print(".");
      display.display();
    }
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    if (display_available) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WiFi Connected");
      display.setCursor(0, 10);
      display.println(WiFi.localIP().toString());
      display.display();
    }
  } else {
    wifi_connected = false;
    Serial.println("WiFi connection failed\!");
    if (display_available) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WiFi Failed\!");
      display.setCursor(0, 10);
      display.println("Check credentials");
      display.display();
    }
  }
  delay(1000);
}

void reconnectMQTT() {
  int attempts = 0;
  
  // Try to connect 3 times, then continue (we'll try again next loop)
  while (\!mqttClient.connected() && attempts < 3) {
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
  if (\!display_available) return;
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("In-Spectre Sensor");
  
  // Sensor status and type
  String sensorName = "Unknown";
  if (sensorType == SENSORTYPE_AS7261) sensorName = "AS7261 (XYZ)";
  else if (sensorType == SENSORTYPE_AS7262) sensorName = "AS7262 (VIS)";
  else if (sensorType == SENSORTYPE_AS7263) sensorName = "AS7263 (NIR)";
  
  display.setCursor(0, 12);
  display.println("Sensor: " + (sensor_available ? sensorName : "Not Found"));
  display.setCursor(0, 24);
  display.println("Temp: " + String(sensor_available ? spectralSensor.getTemperatureF() : 0) + "F");
  
  // Communication mode and status
  if (USE_SERIAL_MODE) {
    display.setCursor(0, 36);
    display.println("Mode: USB Serial");
    display.setCursor(0, 48);
    display.println("Baud: " + String(serialBaudRate));
  } else {
    // Connectivity status
    display.setCursor(0, 36);
    display.println("WiFi: " + String(wifi_connected ? "Connected" : "Disconnected"));
    display.setCursor(0, 48);
    display.println("MQTT: " + String(mqtt_connected ? "Connected" : "Disconnected"));
  }
  
  display.display();
}
