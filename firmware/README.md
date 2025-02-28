# In-Spectre Firmware

This firmware runs on the Heltec ESP32 WiFi LoRa 32 V3 board and communicates with AS7263 and AS7265X spectral sensors via I2C. It sends the collected data to a Flask API server using MQTT.

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

## Communication Setup

The firmware uses two separate communication systems:

1. **WiFi**: Connects the ESP32 to your existing WiFi network
2. **MQTT**: Messaging protocol that runs over the WiFi connection

### WiFi Configuration
The ESP32 connects to your existing WiFi network (like your home router). You need to provide:
- `ssid`: The name of your WiFi network (e.g., "HomeNetwork")
- `password`: Your WiFi password

This allows the ESP32 to connect to the internet through your existing router.

### MQTT Configuration
MQTT is a lightweight messaging protocol that runs over the WiFi connection. You need to set up:

1. **MQTT Broker on your laptop/server**:
   - Install Mosquitto MQTT broker on your laptop or server:
     - **Windows**: Download from https://mosquitto.org/download/
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

4. **Configure the ESP32**:
   - Set `mqtt_server` to your laptop's IP address (e.g., "192.168.1.5")
   - Set `mqtt_port` to 1883 (default MQTT port)

### Complete Data Flow
1. ESP32 connects to your WiFi network
2. ESP32 connects to MQTT broker running on your laptop
3. ESP32 reads sensor data and publishes to MQTT topics
4. Flask API (also on your laptop) subscribes to these topics
5. Web clients request data from the Flask API

## Configuration

Before uploading the firmware, update the following settings in `in_spectre.ino`:

1. WiFi credentials (your existing WiFi network):
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";     // Your home/office WiFi name
   const char* password = "YOUR_WIFI_PASSWORD";  // Your WiFi password
   ```

2. MQTT broker settings (your laptop running Mosquitto):
   ```cpp
   const char* mqtt_server = "YOUR_LAPTOP_IP_ADDRESS";  // e.g., "192.168.1.5"
   const int mqtt_port = 1883;  // Default MQTT port
   ```

3. Update interval (if needed):
   ```cpp
   const unsigned long updateInterval = 5000; // milliseconds
   ```

## Testing MQTT Communication

You can test if the MQTT communication is working using command-line tools:

1. **Subscribe to test topics** (on your laptop):
   ```
   mosquitto_sub -h localhost -t "sensor/as7263" -v
   ```
   In a separate terminal:
   ```
   mosquitto_sub -h localhost -t "sensor/as7265x" -v
   ```

2. **Monitor the data** published by the ESP32 - you should see JSON messages appearing whenever the ESP32 publishes new sensor readings.

## Installation

### Requirements
- Arduino IDE or PlatformIO
- Heltec ESP32 board support
- PubSubClient library
- ArduinoJson library
- Wire library

### Arduino IDE Setup
1. Install the Arduino IDE
2. Add ESP32 board support in Preferences -> Additional Board Manager URLs:
   ```
   https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.1/package_heltec_esp32_index.json
   ```
3. Install the Heltec ESP32 Series boards via Board Manager
4. Install required libraries via Library Manager:
   - PubSubClient
   - ArduinoJson
5. Select the board "Heltec WiFi LoRa 32(V3)"
6. Configure upload speed: 921600
7. Select the correct COM port
8. Click Upload

## Operation

After uploading the firmware, the device will:

1. Initialize the display and show the startup screen
2. Initialize the I2C sensors
3. Connect to WiFi
4. Connect to the MQTT broker
5. Begin taking measurements every 5 seconds
6. Publish data to MQTT topics:
   - `sensor/as7263` for NIR data
   - `sensor/as7265x` for full spectral data

The OLED display will show the current status of the sensors and connections.

## Troubleshooting

- If sensors fail to initialize, check your I2C wiring
- If WiFi connection fails, verify credentials and ensure your ESP32 is within range of your router
- If MQTT connection fails:
  - Check that your laptop IP address is correct
  - Verify that the Mosquitto broker is running on your laptop
  - Make sure your laptop firewall allows connections to port 1883
  - Ensure both devices are on the same WiFi network
- Check the serial monitor (115200 baud) for detailed debugging information