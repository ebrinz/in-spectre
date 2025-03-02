# Direct Display Version of In-Spectre

This version of the In-Spectre firmware bypasses the Heltec library and directly accesses the SSD1306 OLED display using the Adafruit libraries. This can help resolve display issues when the Heltec library is not working correctly.

## Key Differences

1. **Direct Display Control**: Uses Adafruit_SSD1306 and Adafruit_GFX libraries instead of Heltec.display
2. **Graceful Degradation**: Will continue to function even if the display is not available
3. **No LoRa Dependency**: Completely removes the LoRa module dependency
4. **Better Error Reporting**: Provides feedback on display initialization status

## Required Libraries

Before compiling this version, install these libraries:

1. **Adafruit SSD1306**:
   - In Arduino IDE: Tools → Manage Libraries → Search for "Adafruit SSD1306"

2. **Adafruit GFX Library**:
   - In Arduino IDE: Tools → Manage Libraries → Search for "Adafruit GFX"

3. **SparkFun AS726X Arduino Library**:
   - In Arduino IDE: Tools → Manage Libraries → Search for "SparkFun AS726X"
   
4. **PubSubClient**: 
   - In Arduino IDE: Tools → Manage Libraries → Search for "PubSubClient"
   
5. **ArduinoJson**:
   - In Arduino IDE: Tools → Manage Libraries → Search for "ArduinoJson"

## I2C Pin Configuration

The Heltec ESP32 boards can have different I2C pin configurations. This firmware supports multiple pin options:

1. **Option A**: SDA=4, SCL=15 (commonly used on Heltec ESP32 WiFi LoRa 32 V2)
2. **Option B/C**: SDA=21, SCL=22 (standard ESP32 default pins)

The `display_test.ino` sketch will automatically try all these options and tell you which one works for your board. Once you know the correct pins, update the `direct_display.ino` sketch accordingly.

## Board Setup

When uploading this sketch, select the ESP32 Dev Module board type rather than a Heltec-specific board. This ensures more standard initialization:

1. Select board: Tools → Board → ESP32 Arduino → ESP32 Dev Module
2. Partition Scheme: Default 8MB with spiffs
3. Upload Speed: 921600

## Troubleshooting Display Issues

If you're having persistent display issues:

1. Check I2C connections:
   - SDA: GPIO 17
   - SCL: GPIO 18
   
2. Try the hardware reset procedure:
   - Disconnect USB
   - Hold BOOT button
   - Reconnect USB while holding button
   - Keep holding for 5 seconds
   - Release button
   - Upload sketch again
   
3. Try adjusting the I2C speed:
   - In the sketch, modify `Wire.begin(SDA_PIN, SCL_PIN);` to 
     `Wire.begin(SDA_PIN, SCL_PIN, 100000);` to use 100kHz instead of default 400kHz

4. Check the display address:
   - The default is 0x3C, but some displays use 0x3D
   - Modify SCREEN_ADDRESS if needed
