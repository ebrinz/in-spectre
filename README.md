# In-Spectre

A spectral sensor data collection and API system using Heltec ESP32 WiFi LoRa 32 V3 with AS726X spectral sensors.

## Overview

In-Spectre connects spectral sensors to a Flask API, enabling real-time monitoring and analysis of spectral data across UV, visible, and NIR ranges. The system uses:

- **Heltec ESP32 WiFi LoRa 32 V3**: Microcontroller with WiFi connectivity
- **AS726X Sensor**: Auto-detected spectral sensor (AS7261, AS7262, or AS7263)
- **Flexible Communication**: USB Serial or MQTT over WiFi
- **Flask API**: Provides RESTful endpoints for accessing sensor data

## Project Structure

```
in-spectre/
├── api/                  # Python Flask backend
│   ├── routes/           # API endpoints
│   ├── models/           # Data models
│   ├── services/         # Business logic (MQTT & Serial)
│   └── utils/            # Helper functions
├── firmware/             # Arduino code for ESP32
├── tests/                # Test suite
├── docs/                 # Documentation
└── requirements.txt      # Python dependencies
```

## Key Features

- **Auto Sensor Detection**: Works with AS7261, AS7262, or AS7263 sensors
- **Dual Communication Modes**:
  - **USB Serial**: Simple plug-and-play for direct connection
  - **MQTT**: Wireless operation over WiFi
- **Comprehensive Spectral Data**: Covers various wavelength ranges based on the connected sensor
- **Flexible API**: Dynamic switching between communication modes

## Quick Start

### Hardware Setup

1. Connect an AS726X sensor to the ESP32's I2C pins:
   - SDA to GPIO 17
   - SCL to GPIO 18
   - VCC to 3.3V
   - GND to GND
2. Flash the firmware to the ESP32 (Serial mode is enabled by default)
3. Connect the ESP32 to your computer via USB

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

3. Start the Flask server in serial mode:
   ```
   python run.py --mode serial --serial-port [YOUR_PORT]
   ```
   Replace `[YOUR_PORT]` with the actual port (e.g., `/dev/ttyUSB0` or `COM3`)

4. Access the API at http://localhost:5000/api/sensors/status

## API Endpoints

- `GET /api/sensors/status` - Check if API is running
- `GET /api/sensors/spectral` - Get spectral sensor data
- `GET /api/sensors/all` - Get data from all sensors
- `GET /api/sensors/ports` - List available serial ports
- `GET /api/sensors/mode` - Get current communication mode
- `POST /api/sensors/mode` - Set communication mode

## License

MIT

## Contributors

- inSpectral Gadget
