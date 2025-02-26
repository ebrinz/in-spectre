# In-Spectre

A spectral sensor data collection and API system using Heltec ESP32 WiFi LoRa 32 V3 with AS7263 and AS7265X spectral sensors.

## Overview

In-Spectre connects spectral sensors to a Flask API, enabling real-time monitoring and analysis of spectral data across the UV, visible, and NIR ranges. The system uses:

- **Heltec ESP32 WiFi LoRa 32 V3**: Microcontroller with WiFi connectivity
- **AS7263**: 6-channel NIR spectral sensor (610-860nm)
- **AS7265X**: 18-channel spectral sensor covering UV to NIR (410-940nm)
- **MQTT**: For communication between microcontroller and API
- **Flask API**: Provides RESTful endpoints for accessing sensor data

## Project Structure

```
in-spectre/
 api/                  # Python Flask backend
    routes/           # API endpoints
    models/           # Data models
    services/         # Business logic
    utils/            # Helper functions
 firmware/             # Arduino code for ESP32
 tests/                # Test suite
 docs/                 # Documentation
 requirements.txt      # Python dependencies
```

## Getting Started

### Prerequisites

- Python 3.9+
- Arduino IDE or PlatformIO
- MQTT Broker (e.g., Mosquitto)
- AS7263 and AS7265X sensors
- Heltec ESP32 WiFi LoRa 32 V3 board

### API Setup

1. Create a virtual environment:
   ```
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

2. Install dependencies:
   ```
   pip install -r requirements.txt
   ```

3. Start the Flask server:
   ```
   flask run --debug
   ```

### Firmware Setup

1. Open the Arduino IDE
2. Install the required libraries:
   - Heltec ESP32 board support
   - PubSubClient
   - ArduinoJson
3. Open `firmware/in_spectre.ino`
4. Update the WiFi and MQTT settings
5. Upload to your Heltec board

## API Endpoints

- `GET /api/sensors/status` - Check if API is running
- `GET /api/sensors/as7263` - Get data from AS7263 NIR sensor
- `GET /api/sensors/as7265x` - Get data from AS7265X sensor
- `GET /api/sensors/all` - Get data from all sensors
