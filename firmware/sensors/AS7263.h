#ifndef AS7263_H
#define AS7263_H

#include <Wire.h>

// AS7263 I2C Address
#define AS7263_ADDR 0x49

// Register addresses
#define AS7263_STATUS_REG 0x00
#define AS7263_WRITE_REG 0x01
#define AS7263_READ_REG 0x02

// Virtual register addresses
#define AS7263_DEVICE_TYPE 0x00
#define AS7263_HW_VERSION 0x01
#define AS7263_CONTROL_SETUP 0x04
#define AS7263_INT_T 0x05
#define AS7263_TEMPERATURE 0x06
#define AS7263_LED_CONTROL 0x07

#define AS7263_R_DATA 0x08
#define AS7263_S_DATA 0x0A
#define AS7263_T_DATA 0x0C
#define AS7263_U_DATA 0x0E
#define AS7263_V_DATA 0x10
#define AS7263_W_DATA 0x12

#define AS7263_R_CAL 0x14
#define AS7263_S_CAL 0x16
#define AS7263_T_CAL 0x18
#define AS7263_U_CAL 0x1A
#define AS7263_V_CAL 0x1C
#define AS7263_W_CAL 0x1E

// Control bits
#define AS7263_DATA_RDY 0x02
#define AS7263_BANK 0x10
#define AS7263_GAIN_1X 0x00
#define AS7263_GAIN_3X7 0x10
#define AS7263_GAIN_16X 0x20
#define AS7263_GAIN_64X 0x30
#define AS7263_ONE_SHOT 0x01
#define AS7263_DATA_RDY_MASK 0x02

class AS7263 {
public:
    bool begin();
    bool isConnected();
    void takeMeasurements();
    void takeMeasurementsWithBulb();
    float getTemperature();
    float getR();
    float getS();
    float getT();
    float getU();
    float getV();
    float getW();
    void enableBulb(uint8_t intensity = 3);
    void disableBulb();
    void setGain(uint8_t gain);
    void setIntegrationTime(uint8_t integrationValue);
    void setMeasurementMode(uint8_t mode);

private:
    uint8_t virtualReadRegister(uint8_t virtualAddr);
    void virtualWriteRegister(uint8_t virtualAddr, uint8_t value);
    uint16_t readRegister(uint8_t addr);
    bool dataReady();
    void clearDataAvailable();
    void waitForDataReady();
    uint8_t readByte(uint8_t addr);
    uint16_t readUInt(uint8_t addr);
    float readFloat(uint8_t addr);
};

#endif // AS7263_H