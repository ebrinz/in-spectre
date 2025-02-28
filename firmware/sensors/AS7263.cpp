#include "AS7263.h"

bool AS7263::begin() {
  Wire.begin();
  
  // Check if we can communicate with the sensor
  if (!isConnected())
    return false;
    
  // Set default settings
  setIntegrationTime(50); // 50 * 2.8ms = 140ms
  setGain(AS7263_GAIN_1X);
  disableBulb();
  
  return true;
}

bool AS7263::isConnected() {
  // Check if the device responds with the correct device type
  uint8_t deviceType = virtualReadRegister(AS7263_DEVICE_TYPE);
  uint8_t hwVersion = virtualReadRegister(AS7263_HW_VERSION);
  
  // AS7263 should return device type 0x3E
  return (deviceType == 0x3E);
}

void AS7263::takeMeasurements() {
  // Trigger a measurement
  uint8_t value = virtualReadRegister(AS7263_CONTROL_SETUP);
  
  // Set ONE_SHOT bit
  value |= AS7263_ONE_SHOT;
  virtualWriteRegister(AS7263_CONTROL_SETUP, value);
  
  // Wait for data to be ready
  waitForDataReady();
}

void AS7263::takeMeasurementsWithBulb() {
  enableBulb();
  takeMeasurements();
  disableBulb();
}

float AS7263::getTemperature() {
  uint8_t temp = virtualReadRegister(AS7263_TEMPERATURE);
  return (float)temp;
}

float AS7263::getR() {
  return readFloat(AS7263_R_DATA);
}

float AS7263::getS() {
  return readFloat(AS7263_S_DATA);
}

float AS7263::getT() {
  return readFloat(AS7263_T_DATA);
}

float AS7263::getU() {
  return readFloat(AS7263_U_DATA);
}

float AS7263::getV() {
  return readFloat(AS7263_V_DATA);
}

float AS7263::getW() {
  return readFloat(AS7263_W_DATA);
}

void AS7263::enableBulb(uint8_t intensity) {
  if (intensity > 3) intensity = 3;
  
  // LED control register: bits 0:1 control current level
  uint8_t value = intensity;
  virtualWriteRegister(AS7263_LED_CONTROL, value);
}

void AS7263::disableBulb() {
  virtualWriteRegister(AS7263_LED_CONTROL, 0x00);
}

void AS7263::setGain(uint8_t gain) {
  uint8_t value = virtualReadRegister(AS7263_CONTROL_SETUP);
  
  // Clear gain bits and set them
  value &= 0xCF; // Clear bits 4:5
  value |= gain;  // Set new gain
  
  virtualWriteRegister(AS7263_CONTROL_SETUP, value);
}

void AS7263::setIntegrationTime(uint8_t integrationValue) {
  virtualWriteRegister(AS7263_INT_T, integrationValue);
}

void AS7263::setMeasurementMode(uint8_t mode) {
  uint8_t value = virtualReadRegister(AS7263_CONTROL_SETUP);
  
  if (mode == 0) {
    // Set to continuous mode (clear bit 1)
    value &= ~(1 << 1);
  } else {
    // Set to ONE_SHOT mode (set bit 1)
    value |= (1 << 1);
  }
  
  virtualWriteRegister(AS7263_CONTROL_SETUP, value);
}

// Private methods

uint8_t AS7263::virtualReadRegister(uint8_t virtualAddr) {
  uint8_t status;
  
  // Wait for WRITE register to be empty
  do {
    Wire.beginTransmission(AS7263_ADDR);
    Wire.write(AS7263_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7263_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x02) != 0);
  
  // Tell the sensor which virtual register we want to read
  Wire.beginTransmission(AS7263_ADDR);
  Wire.write(AS7263_WRITE_REG);
  Wire.write(virtualAddr);
  Wire.endTransmission();
  
  // Wait for READ register to be full
  do {
    Wire.beginTransmission(AS7263_ADDR);
    Wire.write(AS7263_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7263_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x01) == 0);
  
  // Read the data from the READ register
  Wire.beginTransmission(AS7263_ADDR);
  Wire.write(AS7263_READ_REG);
  Wire.endTransmission();
  
  Wire.requestFrom(AS7263_ADDR, 1);
  return Wire.read();
}

void AS7263::virtualWriteRegister(uint8_t virtualAddr, uint8_t value) {
  uint8_t status;
  
  // Wait for WRITE register to be empty
  do {
    Wire.beginTransmission(AS7263_ADDR);
    Wire.write(AS7263_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7263_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x02) != 0);
  
  // Tell the sensor which virtual register we want to write to
  Wire.beginTransmission(AS7263_ADDR);
  Wire.write(AS7263_WRITE_REG);
  Wire.write(virtualAddr);
  Wire.endTransmission();
  
  // Wait for WRITE register to be empty
  do {
    Wire.beginTransmission(AS7263_ADDR);
    Wire.write(AS7263_STATUS_REG);
    Wire.endTransmission();
    
    Wire.requestFrom(AS7263_ADDR, 1);
    status = Wire.read();
  } while ((status & 0x02) != 0);
  
  // Write the data to the WRITE register
  Wire.beginTransmission(AS7263_ADDR);
  Wire.write(AS7263_WRITE_REG);
  Wire.write(value);
  Wire.endTransmission();
}

uint16_t AS7263::readRegister(uint8_t addr) {
  uint8_t lsb = virtualReadRegister(addr);
  uint8_t msb = virtualReadRegister(addr + 1);
  
  return ((uint16_t)msb << 8) | lsb;
}

bool AS7263::dataReady() {
  uint8_t value = virtualReadRegister(AS7263_CONTROL_SETUP);
  return (value & AS7263_DATA_RDY_MASK) != 0;
}

void AS7263::clearDataAvailable() {
  uint8_t value = virtualReadRegister(AS7263_CONTROL_SETUP);
  
  // Clear DATA_RDY bit
  value &= ~AS7263_DATA_RDY_MASK;
  virtualWriteRegister(AS7263_CONTROL_SETUP, value);
}

void AS7263::waitForDataReady() {
  for (uint8_t i = 0; i < 100; i++) {
    if (dataReady()) return;
    delay(10);
  }
}

uint8_t AS7263::readByte(uint8_t addr) {
  return virtualReadRegister(addr);
}

uint16_t AS7263::readUInt(uint8_t addr) {
  return readRegister(addr);
}

float AS7263::readFloat(uint8_t addr) {
  uint16_t raw = readUInt(addr);
  return (float)raw;
}