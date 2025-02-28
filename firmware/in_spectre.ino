#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "heltec.h"
#include "sensors/AS7263.h"
#include "sensors/AS7265X.h"

// Wi-Fi and MQTT settings - update these to match your network
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_SERVER_IP";
const int mqtt_port = 1883;

// MQTT topics
const char* topic_as7263 = "sensor/as7263";
const char* topic_as7265x = "sensor/as7265x";

// Update interval (milliseconds)
const unsigned long updateInterval = 5000;
unsigned long lastUpdateTime = 0;

// Create sensor objects
AS7263 as7263;
AS7265X as7265x;

// Create WiFi and MQTT clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// I2C pins for Heltec WiFi LoRa 32 V3
#define SDA_PIN 17
#define SCL_PIN 18

// I2C multiplexer address and channel selection
#define TCA9548A_ADDRESS 0x70
#define AS7263_CHANNEL 0
#define AS7265X_CHANNEL 1

// Status flags
bool as7263_available = false;
bool as7265x_available = false;

void setup() {
  // Initialize Heltec board (display, serial, etc.)
  Heltec.begin(true /*DisplayEnable*/, true /*LoRaEnable*/, true /*SerialEnable*/, true /*PABOOST*/, 470E6 /*BAND*/);
  
  // Display setup message
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "In-Spectre");
  Heltec.display->drawString(0, 10, "Initializing...");
  Heltec.display->display();
  
  // Set up I2C with the correct pins for Heltec V3
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Initialize sensors
  Heltec.display->drawString(0, 20, "Initializing sensors...");
  Heltec.display->display();
  
  // Try to initialize AS7263
  selectTCAChannel(AS7263_CHANNEL);
  as7263_available = as7263.begin();
  
  // Try to initialize AS7265X
  selectTCAChannel(AS7265X_CHANNEL);
  as7265x_available = as7265x.begin();
  
  // Display sensor status
  Heltec.display->drawString(0, 30, "AS7263: " + String(as7263_available ? "OK" : "FAIL"));
  Heltec.display->drawString(0, 40, "AS7265X: " + String(as7265x_available ? "OK" : "FAIL"));
  Heltec.display->display();
  delay(1000);
  
  // Connect to WiFi
  setupWifi();
  
  // Connect to MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  
  // Final setup message
  updateDisplay();
}

void loop() {
  // Ensure MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  
  // Update sensor readings at the specified interval
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentMillis;
    
    // Read sensor data and publish to MQTT
    if (as7263_available) {
      readAndPublishAS7263();
    }
    
    if (as7265x_available) {
      readAndPublishAS7265X();
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
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "WiFi Connected");
    Heltec.display->drawString(0, 10, WiFi.localIP().toString());
    Heltec.display->display();
  } else {
    Serial.println("WiFi connection failed!");
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "WiFi Failed!");
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
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
      attempts++;
    }
  }
}

// I2C Multiplexer (TCA9548A) control
void selectTCAChannel(uint8_t channel) {
  if (channel > 7) return;
  
  Wire.beginTransmission(TCA9548A_ADDRESS);
  Wire.write(1 << channel);
  Wire.endTransmission();
  delay(10); // Small delay to ensure channel is ready
}

// AS7263 Sensor Functions
void readAndPublishAS7263() {
  // Select the correct I2C channel
  selectTCAChannel(AS7263_CHANNEL);
  
  // Take a measurement
  as7263.takeMeasurements();
  
  // Create a JSON document for the sensor data
  StaticJsonDocument<256> doc;
  doc["timestamp"] = millis() / 1000.0;
  doc["temperature"] = as7263.getTemperature();
  
  // Read the 6 channels
  doc["r"] = as7263.getR(); // 610nm
  doc["s"] = as7263.getS(); // 680nm
  doc["t"] = as7263.getT(); // 730nm
  doc["u"] = as7263.getU(); // 760nm
  doc["v"] = as7263.getV(); // 810nm
  doc["w"] = as7263.getW(); // 860nm
  
  // Publish to MQTT
  char buffer[256];
  serializeJson(doc, buffer);
  mqttClient.publish(topic_as7263, buffer);
  
  Serial.println("Published AS7263 data");
}

// AS7265X Sensor Functions
void readAndPublishAS7265X() {
  // Select the correct I2C channel
  selectTCAChannel(AS7265X_CHANNEL);
  
  // Take a measurement
  as7265x.takeMeasurements();
  
  // Create a JSON document for the sensor data
  StaticJsonDocument<512> doc;
  doc["timestamp"] = millis() / 1000.0;
  doc["temperature"] = as7265x.getTemperature();
  
  // Read all 18 channels
  doc["a"] = as7265x.getA(); // 410nm
  doc["b"] = as7265x.getB(); // 435nm
  doc["c"] = as7265x.getC(); // 460nm
  doc["d"] = as7265x.getD(); // 485nm
  doc["e"] = as7265x.getE(); // 510nm
  doc["f"] = as7265x.getF(); // 535nm
  doc["g"] = as7265x.getG(); // 560nm
  doc["h"] = as7265x.getH(); // 585nm
  doc["i"] = as7265x.getI(); // 610nm
  doc["j"] = as7265x.getJ(); // 645nm
  doc["k"] = as7265x.getK(); // 680nm
  doc["l"] = as7265x.getL(); // 705nm
  doc["m"] = as7265x.getM(); // 730nm (alias for R)
  doc["n"] = as7265x.getN(); // 760nm (alias for S)
  doc["o"] = as7265x.getO(); // 810nm (alias for T)
  doc["p"] = as7265x.getP(); // 860nm (alias for U)
  doc["q"] = as7265x.getQ(); // 900nm (alias for V)
  doc["r"] = as7265x.getR(); // 940nm (alias for W)
  
  // Publish to MQTT
  char buffer[512];
  serializeJson(doc, buffer);
  mqttClient.publish(topic_as7265x, buffer);
  
  Serial.println("Published AS7265X data");
}

// Display functions
void updateDisplay() {
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "In-Spectre Sensors");
  
  // Sensor status
  Heltec.display->drawString(0, 12, "AS7263: " + String(as7263_available ? "Online" : "N/A"));
  Heltec.display->drawString(0, 24, "AS7265X: " + String(as7265x_available ? "Online" : "N/A"));
  
  // Connectivity status
  Heltec.display->drawString(0, 36, "WiFi: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected"));
  Heltec.display->drawString(0, 48, "MQTT: " + String(mqttClient.connected() ? "Connected" : "Disconnected"));
  
  Heltec.display->display();
}