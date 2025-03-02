// Basic test sketch for Heltec ESP32 OLED display
// This bypasses the Heltec library and uses the SSD1306 library directly

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // I2C address of the OLED display

// I2C pins for Heltec ESP32 - try both options
// Option A - For older Heltec boards
#define SDA_PIN_A 4
#define SCL_PIN_A 15

// Option B - For newer Heltec boards
#define SDA_PIN_B 21
#define SCL_PIN_B 22

// Option C - Generic ESP32 defaults
#define SDA_PIN_C 21
#define SCL_PIN_C 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int currentOption = 0;
bool displayWorking = false;

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial monitor time to open
  Serial.println("\n\nHeltec Display Test");
  
  // Try all three pin configurations
  tryDisplayInit(SDA_PIN_A, SCL_PIN_A, "Option A (4,15)");
  if (\!displayWorking) tryDisplayInit(SDA_PIN_B, SCL_PIN_B, "Option B (21,22)"); 
  if (\!displayWorking) tryDisplayInit(SDA_PIN_C, SCL_PIN_C, "Option C (21,22 alternate)");
  
  if (displayWorking) {
    // Display test patterns
    testDisplay();
  } else {
    Serial.println("Could not initialize the display with any pin configuration.");
    Serial.println("Please check your hardware connections and try again.");
  }
}

void loop() {
  // Nothing to do here
}

bool tryDisplayInit(int sda, int scl, String option) {
  Serial.println("\nTrying " + option);
  Serial.println("SDA: " + String(sda) + ", SCL: " + String(scl));
  
  // Reset Wire library
  Wire.end();
  delay(100);
  
  // Initialize I2C
  Wire.begin(sda, scl);
  delay(100);
  
  // Try to initialize the display
  Serial.println("Initializing display...");
  if(display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("✓ Display initialized successfully with " + option + "\!");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Display working\!");
    display.setCursor(0, 10);
    display.println("Using " + option);
    display.setCursor(0, 20);
    display.println("SDA: " + String(sda));
    display.setCursor(0, 30);
    display.println("SCL: " + String(scl));
    display.display();
    delay(2000);
    
    currentOption = option.charAt(7) - 'A' + 1; // Extract option number
    displayWorking = true;
    return true;
  } else {
    Serial.println("✗ Display initialization failed with " + option);
    delay(500);
    return false;
  }
}

void testDisplay() {
  // Clear the buffer
  display.clearDisplay();
  
  // Display text
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Heltec Display Test");
  display.setCursor(0, 10);
  display.println("Option " + String(currentOption) + " Working\!");
  display.setCursor(0, 25);
  display.println("If you can see this,");
  display.setCursor(0, 35);
  display.println("your display works\!");
  display.display();
  delay(2000);
  
  // Draw a pattern
  for(int i=0; i<display.width(); i+=8) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(10);
  }
  for(int i=0; i<display.height(); i+=8) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(10);
  }
  delay(1000);
  
  // Invert and normalize display
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
  
  // Final message
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Display test");
  display.setCursor(0, 10);
  display.println("completed\!");
  display.setCursor(0, 30);
  display.println("Remember Option " + String(currentOption));
  display.setCursor(0, 40);
  display.println("for your project");
  display.display();
}
