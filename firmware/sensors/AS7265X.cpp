#include "AS7265X.h"

bool AS7265X::begin() {
  Wire.begin();
  
  // Check if we can communicate with the sensor
  if (!isConnected())
    return false;
  
  // Set default configuration
  setIntegrationTime(50);  // 50 * 2.8ms = 140ms
  setGain(AS7265X_GAIN_1X);
  disableBulb();
  
  return true;
}

bool AS7265X::isConnected() {
  // AS7265x has a hardware version register
  uint8_t hardwareVersion = virtualReadRegister(AS7265X_HW_VERSION);
  
  // We expect hardware version to be non-zero and compatible
  return (hardwareVersion != 0);
}

float AS7265X::getTemperature() {
  return (float)virtualReadRegister(AS7265X_DEVICE_TEMP);
}

void AS7265X::takeMeasurements() {
  // Set bit 0 in CONFIG to trigger a measurement
  uint8_t value = virtualReadRegister(AS7265X_CONFIG);
  value |= 0x01;  // Set the DATA_RDY bit
  virtualWriteRegister(AS7265X_CONFIG, value);
  
  // Wait for data to be ready
  waitForDataReady();
}

void AS7265X::takeMeasurementsWithBulb(uint8_t bulbType) {
  enableBulb(bulbType);
  takeMeasurements();
  disableBulb();
}

float AS7265X::getChannel(uint8_t channelIndex) {
  uint16_t raw;
  uint8_t device, reg;
  
  if (channelIndex < 6) {
    // First sensor: channels A-F (UV)
    device = 0;
    reg = AS7265X_UV_A + (channelIndex * 2);
  } else if (channelIndex < 12) {
    // Second sensor: channels G-L (VIS)
    device = 1;
    reg = AS7265X_VIS_G + ((channelIndex - 6) * 2);
  } else if (channelIndex < 18) {
    // Third sensor: channels R-W (NIR)
    device = 2;
    reg = AS7265X_NIR_R + ((channelIndex - 12) * 2);
  } else {
    return 0;  // Invalid channel
  }
  
  raw = readRegister(device, reg);
  return convertBytesToFloat(raw);
}

// Individual channel getters
float AS7265X::getA() { return getChannel(0); }  // 410nm
float AS7265X::getB() { return getChannel(1); }  // 435nm
float AS7265X::getC() { return getChannel(2); }  // 460nm
float AS7265X::getD() { return getChannel(3); }  // 485nm
float AS7265X::getE() { return getChannel(4); }  // 510nm
float AS7265X::getF() { return getChannel(5); }  // 535nm

float AS7265X::getG() { return getChannel(6); }  // 560nm
float AS7265X::getH() { return getChannel(7); }  // 585nm
float AS7265X::getI() { return getChannel(8); }  // 610nm
float AS7265X::getJ() { return getChannel(9); }  // 645nm
float AS7265X::getK() { return getChannel(10); } // 680nm
float AS7265X::getL() { return getChannel(11); } // 705nm

// In AS7265x the NIR channels are R-W, but to match popular
// spectroscopy conventions we alias them as M-R
float AS7265X::getM() { return getChannel(12); } // 730nm (same as R)
float AS7265X::getN() { return getChannel(13); } // 760nm (same as S)
float AS7265X::getO() { return getChannel(14); } // 810nm (same as T)
float AS7265X::getP() { return getChannel(15); } // 860nm (same as U)
float AS7265X::getQ() { return getChannel(16); } // 900nm (same as V)
float AS7265X::getR() { return getChannel(17); } // 940nm (same as W)

// For compatibility with AS7263, which uses R-W naming
float AS7265X::getR_AS7263() { return getI(); } // 610nm
float AS7265X::getS_AS7263() { return getK(); } // 680nm
float AS7265X::getT_AS7263() { return getM(); } // 730nm
float AS7265X::getU_AS7263() { return getN(); } // 760nm
float AS7265X::getV_AS7263() { return getO(); } // 810nm
float AS7265X::getW_AS7263() { return getP(); } // 860nm

void AS7265X::setIntegrationTime(uint8_t time) {
  // Set the integration time for all three devices
  virtualWriteRegister(AS7265X_INTEG_TIME, time);
}

void AS7265X::setGain(uint8_t gain) {
  // Set the gain the same for all devices
  uint8_t value = virtualReadRegister(AS7265X_CONFIG);
  
  // Clear gain bits (bits 4:5)
  value &= 0xCF;
  
  // Set the new gain
  value |= (gain << 4);
  
  virtualWriteRegister(AS7265X_CONFIG, value);
}

void AS7265X::setBulbCurrent(uint8_t current) {
  if (current > 3) current = 3; // Limit to valid range
  
  uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG);
  
  // Set current for all bulb types (bits 0:1 for white, 2:3 for IR, 4:5 for UV)
  // Current levels: 00=12.5mA, 01=25mA, 10=50mA, 11=100mA
  value &= 0xC0; // Clear all current settings
  value |= (current | (current << 2) | (current << 4));
  
  virtualWriteRegister(AS7265X_LED_CONFIG, value);
}

void AS7265X::enableBulb(uint8_t type) {
  uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG);
  
  // Clear bits 6:7 (LED indicators and bulbs)
  value &= 0x3F;
  
  switch (type) {
    case AS7265X_LED_WHITE:
      value |= 0x40; // Set bit 6
      break;
    case AS7265X_LED_UV:
      value |= 0x80; // Set bit 7
      break;
    case AS7265X_LED_IR:
      value |= 0xC0; // Set bits 6 and 7
      break;
  }
  
  virtualWriteRegister(AS7265X_LED_CONFIG, value);
}

void AS7265X::disableBulb() {
  uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG);
  
  // Clear bits 6:7 (disable all bulbs)
  value &= 0x3F;
  
  virtualWriteRegister(AS7265X_LED_CONFIG, value);
}

// Private methods

uint8_t AS7265X::virtualReadRegister(uint8_t virtualAddr) {
  uint8_t status;
  
  // Wait for WRITE register to be empty
  do {
    Wire.beginTransmission(AS7265X_ADDR);
    Wire.write(AS7265X_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7265X_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x02) != 0);
  
  // Tell the sensor which virtual register we want to read
  Wire.beginTransmission(AS7265X_ADDR);
  Wire.write(AS7265X_WRITE_REG);
  Wire.write(virtualAddr);
  Wire.endTransmission();
  
  // Wait for READ register to be full
  do {
    Wire.beginTransmission(AS7265X_ADDR);
    Wire.write(AS7265X_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7265X_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x01) == 0);
  
  // Read the data from the READ register
  Wire.beginTransmission(AS7265X_ADDR);
  Wire.write(AS7265X_READ_REG);
  Wire.endTransmission();
  
  Wire.requestFrom(AS7265X_ADDR, 1);
  return Wire.read();
}

void AS7265X::virtualWriteRegister(uint8_t virtualAddr, uint8_t value) {
  uint8_t status;
  
  // Wait for WRITE register to be empty
  do {
    Wire.beginTransmission(AS7265X_ADDR);
    Wire.write(AS7265X_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7265X_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x02) != 0);
  
  // Tell the sensor which virtual register we want to write to
  Wire.beginTransmission(AS7265X_ADDR);
  Wire.write(AS7265X_WRITE_REG);
  Wire.write(virtualAddr);
  Wire.endTransmission();
  
  // Wait for WRITE register to be empty
  do {
    Wire.beginTransmission(AS7265X_ADDR);
    Wire.write(AS7265X_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7265X_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x02) != 0);
  
  // Write the data to the WRITE register
  Wire.beginTransmission(AS7265X_ADDR);
  Wire.write(AS7265X_WRITE_REG);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t AS7265X::bankReadRegister(uint8_t device, uint8_t reg) {
  if (device > 2) return 0;
  
  // Set the correct bank
  virtualWriteRegister(AS7265X_CONFIG, device);
  
  return virtualReadRegister(reg);
}

void AS7265X::bankWriteRegister(uint8_t device, uint8_t reg, uint8_t value) {
  if (device > 2) return;
  
  // Set the correct bank
  virtualWriteRegister(AS7265X_CONFIG, device);
  
  virtualWriteRegister(reg, value);
}

uint16_t AS7265X::readRegister(uint8_t device, uint8_t addr) {
  uint8_t lsb = bankReadRegister(device, addr);
  uint8_t msb = bankReadRegister(device, addr + 1);
  
  return ((uint16_t)msb << 8) | lsb;
}

bool AS7265X::dataReady() {
  uint8_t value = virtualReadRegister(AS7265X_CONFIG);
  return (value & 0x02) == 0; // Data ready when bit 1 is clear
}

void AS7265X::waitForDataReady() {
  for (uint8_t i = 0; i < 100; i++) {
    if (dataReady()) return;
    delay(10);
  }
}

float AS7265X::convertBytesToFloat(uint16_t raw) {
  // Convert the raw 16-bit value to a calibrated float value
  return (float)raw;
}

void AS7265X::enableIndicator() {
  uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG);
  
  // Set bit 3 to enable indicator LED
  value |= (1 << 3);
  
  virtualWriteRegister(AS7265X_LED_CONFIG, value);
}

void AS7265X::disableIndicator() {
  uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG);
  
  // Clear bit 3 to disable indicator LED
  value &= ~(1 << 3);
  
  virtualWriteRegister(AS7265X_LED_CONFIG, value);
}