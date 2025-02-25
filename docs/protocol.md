# In-Spectre Communication Protocol

## Overview
This document describes the communication protocol between the Heltec ESP32 microcontroller and the Flask API server. The system uses MQTT for reliable, lightweight messaging.

## MQTT Topics

### Sensor Data Topics
- `sensor/as7263` - NIR spectral data from AS7263 sensor
- `sensor/as7265x` - Full spectral data from AS7265X sensor

## Message Format
All messages are formatted as JSON objects.

### AS7263 Message Format
```json
{
  "timestamp": 1234567890.123,
  "temperature": 25.5,
  "r": 120.5,  // 610nm
  "s": 240.3,  // 680nm
  "t": 350.2,  // 730nm
  "u": 175.8,  // 760nm
  "v": 280.1,  // 810nm
  "w": 310.7   // 860nm
}
```

### AS7265X Message Format
```json
{
  "timestamp": 1234567890.123,
  "temperature": 25.5,
  "a": 82.5,   // 410nm (UV)
  "b": 76.3,   // 435nm
  "c": 91.2,   // 460nm
  "d": 120.8,  // 485nm
  "e": 180.1,  // 510nm
  "f": 210.7,  // 535nm
  "g": 240.5,  // 560nm
  "h": 220.3,  // 585nm
  "i": 200.2,  // 610nm
  "j": 180.8,  // 645nm
  "k": 160.1,  // 680nm
  "l": 140.7,  // 705nm
  "m": 120.5,  // 730nm
  "n": 100.3,  // 760nm
  "o": 80.2,   // 810nm
  "p": 60.8,   // 860nm
  "q": 40.1,   // 900nm
  "r": 20.7    // 940nm
}
```

## Communication Flow
1. ESP32 connects to WiFi network
2. ESP32 connects to MQTT broker
3. ESP32 reads sensor data at regular intervals (default: 5 seconds)
4. ESP32 publishes data to appropriate MQTT topics
5. Flask API subscribes to MQTT topics and caches the latest readings
6. Client applications request data from Flask API endpoints

## API Endpoints

### GET /api/sensors/status
Check if API is running.

### GET /api/sensors/as7263
Get the latest data from the AS7263 NIR spectral sensor.

### GET /api/sensors/as7265x
Get the latest data from the AS7265X 18-channel spectral sensor.

### GET /api/sensors/all
Get data from all sensors.

## Error Handling
- If sensor data is older than 30 seconds, the API will return a 500 error
- If MQTT connection is lost, the ESP32 will automatically attempt to reconnect
- If WiFi connection is lost, the ESP32 will automatically attempt to reconnect