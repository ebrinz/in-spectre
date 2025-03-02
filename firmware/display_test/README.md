# Display Test for Heltec ESP32

This is a simple diagnostic sketch to test if the OLED display on your Heltec ESP32 board is functioning correctly.

## Purpose

This test:
1. Bypasses the Heltec library entirely
2. Uses the Adafruit SSD1306 library to communicate directly with the display
3. Performs multiple display tests to identify any issues

## Required Libraries

Before compiling and uploading this sketch, install these libraries:

1. **Adafruit SSD1306**:
   - In Arduino IDE: Tools → Manage Libraries → Search for "Adafruit SSD1306"

2. **Adafruit GFX Library**:
   - In Arduino IDE: Tools → Manage Libraries → Search for "Adafruit GFX"

## Instructions

1. Install the required libraries
2. Connect your Heltec ESP32 board to your computer
3. Select the correct board from the Arduino IDE board menu:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module (not Heltec specific)
4. Set the correct upload settings:
   - Upload Speed: 921600
   - CPU Frequency: 240MHz
   - Flash Frequency: 80MHz
   - Flash Mode: QIO
   - Flash Size: 8MB
   - Partition Scheme: Default 8MB with spiffs
5. Select the correct COM port
6. Upload the sketch
7. Open Serial Monitor at 115200 baud to see diagnostic messages

## Interpreting Results

- If the display shows the test patterns, the display hardware is working correctly
- If the serial output shows "Display initialized successfully\!" but nothing appears on the screen, there may be an issue with the display controller or connections
- If initialization fails, there might be an I2C communication error

## Hard Reset Procedure

If the test fails, try this hardware reset procedure:

1. Disconnect the board from USB
2. Hold down the "BOOT" button
3. While holding the button, reconnect USB
4. Keep holding for ~5 seconds
5. Release the button
6. Try uploading the sketch again
