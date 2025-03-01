# In-Spectre

A spectral sensor data collection and API system using Heltec ESP32 WiFi LoRa 32 V3 with AS7263 and AS7265X spectral sensors.

## Overview

In-Spectre connects spectral sensors to a Flask API, enabling real-time monitoring and analysis of spectral data across the UV, visible, and NIR ranges. The system uses:

- **Heltec ESP32 WiFi LoRa 32 V3**: Microcontroller with WiFi connectivity
- **AS7263**: 6-channel NIR spectral sensor (610-860nm)
- **AS7265X**: 18-channel spectral sensor covering UV to NIR (410-940nm)
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
│   └── sensors/          # Sensor libraries
├── tests/                # Test suite
├── docs/                 # Documentation
└── requirements.txt      # Python dependencies
```

## Key Features

- **Dual Communication Modes**:
  - **USB Serial**: Simple plug-and-play for direct connection
  - **MQTT**: Wireless operation over WiFi

- **Comprehensive Spectral Data**: 
  - 6 channels from AS7263 (NIR)
  - 18 channels from AS7265X (UV to NIR)

- **Flexible API**: 
  - RESTful endpoints for data access
  - Dynamic switching between communication modes

## Quick Start

### Hardware Setup

1. Connect the sensors to the ESP32 using the TCA9548A I2C multiplexer
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
- `GET /api/sensors/as7263` - Get data from AS7263 NIR sensor
- `GET /api/sensors/as7265x` - Get data from AS7265X sensor
- `GET /api/sensors/all` - Get data from all sensors
- `GET /api/sensors/ports` - List available serial ports
- `GET /api/sensors/mode` - Get current communication mode
- `POST /api/sensors/mode` - Set communication mode

## License

MIT

## Contributors

- inSpectral Gadget