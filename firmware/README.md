# In-Spectre Firmware

This firmware runs on the Heltec ESP32 WiFi LoRa 32 V3 board and communicates with AS726X spectral sensors via I2C. It can send data via USB serial or MQTT over WiFi.

## Required Libraries

Before compiling the firmware, you need to install these libraries:

1. **SparkFun AS726X Arduino Library**: For AS726X spectral sensor support
   - In Arduino IDE: Tools → Manage Libraries → Search for "SparkFun AS726X" and install it
   
2. **PubSubClient**: For MQTT communication
   - In Arduino IDE: Tools → Manage Libraries → Search for "PubSubClient" by Nick O'Leary
   
3. **ArduinoJson**: For JSON formatting
   - In Arduino IDE: Tools → Manage Libraries → Search for "ArduinoJson" by Benoit Blanchon

4. **Heltec ESP32 Dev-Boards**: 
   - In Arduino IDE: Tools → Manage Libraries → Search for "Heltec ESP32 Dev-Boards" and install it

## Hardware Setup

### Required Components
- Heltec ESP32 WiFi LoRa 32 V3 board
- AS726X spectral sensor breakout (AS7261, AS7262, or AS7263)
- Connecting wires

### Wiring
Connect the components as follows:

1. **Heltec ESP32 WiFi LoRa 32 V3**:
   - SDA: GPIO 17
   - SCL: GPIO 18
   - 3.3V to VCC on the sensor
   - GND to GND on the sensor

2. **AS726X Sensor**:
   - VCC to 3.3V
   - GND to GND
   - SDA to ESP32 SDA (GPIO 17)
   - SCL to ESP32 SCL (GPIO 18)

## About the Sensors

This project supports any of SparkFun's AS726X spectral sensors:

- **AS7261**: XYZ color sensor with NIR channels
  - Color coordinates plus NIR, Dark and Clear channels

- **AS7262**: 6-channel Visible light spectral sensor
  - Wavelengths: 450nm (V), 500nm (B), 550nm (G), 570nm (Y), 600nm (O), 650nm (R)
  
- **AS7263**: 6-channel Near-Infrared (NIR) spectral sensor
  - Wavelengths: 610nm (R), 680nm (S), 730nm (T), 760nm (U), 810nm (V), 860nm (W)

The firmware auto-detects which sensor type is connected and reads the appropriate channels.

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
   mosquitto_sub -h localhost -t "sensor/spectral" -v
   ```

2. **Monitor the data** published by the ESP32

## Installation

### Arduino IDE Setup
1. Install the Arduino IDE
2. Install the required libraries via Library Manager (see Required Libraries section)
3. Select the board "Heltec WiFi LoRa 32(V3)"
4. Configure upload speed: 921600
5. Select the correct COM port
6. Click Upload

## Troubleshooting

### Compilation Errors
- **Error: AS726X.h: No such file or directory**:
  - Install the SparkFun AS726X Arduino Library in Arduino IDE (Tools → Manage Libraries)
  
- **Error: PubSubClient.h: No such file or directory**:
  - Install the PubSubClient library in Arduino IDE (Tools → Manage Libraries)
  
- **Error: ArduinoJson.h: No such file or directory**:
  - Install the ArduinoJson library in Arduino IDE (Tools → Manage Libraries)

- **Error: "heltec.h": No such file or directory**:
  - Install the Heltec ESP32 Dev-Boards library in Arduino IDE (Tools → Manage Libraries)

### Runtime Issues
- If sensor fails to initialize, check your I2C wiring
- If using Serial mode and no data appears:
  - Verify the correct port is selected
  - Check baud rate (115200)
  - Try pressing the reset button on the ESP32
- If using MQTT mode and no data appears:
  - Ensure WiFi credentials are correct
  - Verify MQTT broker is running and accessible
  - Check your laptop's firewall settings
- Check the serial monitor and OLED display for status information