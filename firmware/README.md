# In-Spectre Firmware

This firmware runs on the Heltec ESP32 WiFi LoRa 32 V3 board and communicates with AS7263 and AS7265X spectral sensors via I2C. It can send data via USB serial or MQTT over WiFi.

## Hardware Setup

### Required Components
- Heltec ESP32 WiFi LoRa 32 V3 board
- AS7263 NIR spectral sensor breakout
- AS7265X 18-channel spectral sensor breakout
- TCA9548A I2C multiplexer (to manage both sensors on the same I2C bus)
- Connecting wires

### Wiring
Connect the components as follows:

1. **Heltec ESP32 WiFi LoRa 32 V3**:
   - SDA: GPIO 17
   - SCL: GPIO 18
   - 3.3V to all VCC pins
   - GND to all GND pins

2. **TCA9548A I2C Multiplexer**:
   - VCC to 3.3V
   - GND to GND
   - SDA to ESP32 SDA (GPIO 17)
   - SCL to ESP32 SCL (GPIO 18)
   - A0, A1, A2 to GND (sets address to 0x70)

3. **AS7263 Sensor**:
   - VCC to 3.3V
   - GND to GND
   - SDA to TCA9548A SC0
   - SCL to TCA9548A SD0

4. **AS7265X Sensor**:
   - VCC to 3.3V
   - GND to GND
   - SDA to TCA9548A SC1
   - SCL to TCA9548A SD1

## Communication Options

The firmware supports two communication methods:

### 1. Serial-over-USB (Recommended for Quick Setup)
- Connect the ESP32 to your computer via USB
- No additional setup required - works out of the box
- Data is streamed as JSON over the USB serial connection
- Set `USE_SERIAL_MODE` to `true` in the firmware

### 2. MQTT over WiFi
- ESP32 connects to your WiFi network and an MQTT broker
- More complex setup but allows wireless operation
- Set `USE_SERIAL_MODE` to `false` in the firmware

## Configuration

Before uploading the firmware, update the settings in `in_spectre.ino`:

1. Set communication mode:
   ```cpp
   #define USE_SERIAL_MODE true   // Use USB Serial (easy setup)
   // #define USE_SERIAL_MODE false  // Use MQTT over WiFi
   ```

2. If using MQTT mode, configure WiFi and MQTT:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";     // Your home/office WiFi name
   const char* password = "YOUR_WIFI_PASSWORD";  // Your WiFi password
   const char* mqtt_server = "YOUR_LAPTOP_IP_ADDRESS";  // e.g., "192.168.1.5"
   const int mqtt_port = 1883;  // Default MQTT port
   ```

3. Update interval if needed:
   ```cpp
   const unsigned long updateInterval = 5000; // milliseconds
   ```

## MQTT Setup (Only for MQTT Mode)

If you're using MQTT mode, you'll need to:

1. **Install MQTT Broker on your laptop/server**:
   - **Windows**: Download Mosquitto from https://mosquitto.org/download/
   - **macOS**: `brew install mosquitto`
   - **Linux**: `sudo apt install mosquitto`

2. **Configure the broker**:
   - Edit mosquitto.conf to allow connections:
     ```
     listener 1883
     allow_anonymous true
     ```
   - Start the broker:
     - **Windows**: Run as a service or `mosquitto -c mosquitto.conf`
     - **macOS/Linux**: `mosquitto -c /path/to/mosquitto.conf`

3. **Find your laptop's IP address**:
   - **Windows**: Run `ipconfig` in Command Prompt
   - **macOS/Linux**: Run `ifconfig` or `ip addr` in Terminal
   - Use the IP address of your WiFi or Ethernet interface (usually starts with 192.168.x.x)

## Testing Communication

### Testing Serial Mode
1. Connect the ESP32 to your computer via USB
2. Open a serial monitor (Arduino IDE, PlatformIO, or separate app)
3. Set baud rate to 115200
4. You should see JSON-formatted data appearing at regular intervals

### Testing MQTT Mode
You can test if the MQTT communication is working using command-line tools:

1. **Subscribe to test topics** (on your laptop):
   ```
   mosquitto_sub -h localhost -t "sensor/as7263" -v
   ```
   In a separate terminal:
   ```
   mosquitto_sub -h localhost -t "sensor/as7265x" -v
   ```

2. **Monitor the data** published by the ESP32

## Installation

### Requirements
- Arduino IDE or PlatformIO
- Heltec ESP32 board support
- Required libraries:
  - PubSubClient (for MQTT)
  - ArduinoJson
  - Wire (I2C)

### Arduino IDE Setup
1. Install the Arduino IDE
2. Add ESP32 board support in Preferences -> Additional Board Manager URLs:
   ```
   https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.1/package_heltec_esp32_index.json
   ```
3. Install the Heltec ESP32 Series boards via Board Manager
4. Install required libraries via Library Manager
5. Select the board "Heltec WiFi LoRa 32(V3)"
6. Configure upload speed: 921600
7. Select the correct COM port
8. Click Upload

## Flask API Configuration

The Flask API can be configured to use either communication method:

### Serial Mode
```
python run.py --mode serial --serial-port /dev/ttyUSB0
```

### MQTT Mode
```
python run.py --mode mqtt --mqtt-broker localhost
```

## Troubleshooting

- If sensors fail to initialize, check your I2C wiring
- If using Serial mode and no data appears:
  - Verify the correct port is selected
  - Check baud rate (115200)
  - Try pressing the reset button on the ESP32
- If using MQTT mode and no data appears:
  - Ensure WiFi credentials are correct
  - Verify MQTT broker is running and accessible
  - Check your laptop's firewall settings
- Check the serial monitor and OLED display for status information