#ifndef AS7265X_H
#define AS7265X_H

#include <Wire.h>

// AS7265X I2C Address
#define AS7265X_ADDR 0x49

// Register addresses
#define AS7265X_STATUS_REG 0x00
#define AS7265X_WRITE_REG 0x01
#define AS7265X_READ_REG 0x02

// Virtual register addresses
#define AS7265X_HW_VERSION 0x01
#define AS7265X_FW_VERSION_H 0x02
#define AS7265X_FW_VERSION_L 0x03
#define AS7265X_CONFIG 0x04
#define AS7265X_INTEG_TIME 0x05
#define AS7265X_DEVICE_TEMP 0x06
#define AS7265X_LED_CONFIG 0x07

// Data registers (18 channels)
#define AS7265X_R_G_A 0x08 // A (410nm), G (560nm), R (610nm)
#define AS7265X_S_H_B 0x0A // B (435nm), H (585nm), S (680nm)
#define AS7265X_T_I_C 0x0C // C (460nm), I (610nm), T (730nm)
#define AS7265X_U_J_D 0x0E // D (485nm), J (645nm), U (760nm)
#define AS7265X_V_K_E 0x10 // E (510nm), K (680nm), V (810nm)
#define AS7265X_W_L_F 0x12 // F (535nm), L (705nm), W (860nm)
#define AS7265X_SLAVE_STATUS_REG 0x13
#define AS7265X_SLAVE_WRITE_REG 0x14
#define AS7265X_SLAVE_READ_REG 0x15

// Data from AS72652 (UV)
#define AS7265X_UV_A 0x08
#define AS7265X_UV_B 0x0A
#define AS7265X_UV_C 0x0C
#define AS7265X_UV_D 0x0E
#define AS7265X_UV_E 0x10
#define AS7265X_UV_F 0x12

// Data from AS72651 (Visible)
#define AS7265X_VIS_G 0x08
#define AS7265X_VIS_H 0x0A
#define AS7265X_VIS_I 0x0C
#define AS7265X_VIS_J 0x0E
#define AS7265X_VIS_K 0x10
#define AS7265X_VIS_L 0x12

// Data from AS72653 (NIR)
#define AS7265X_NIR_R 0x08
#define AS7265X_NIR_S 0x0A
#define AS7265X_NIR_T 0x0C
#define AS7265X_NIR_U 0x0E
#define AS7265X_NIR_V 0x10
#define AS7265X_NIR_W 0x12

// Gain settings
#define AS7265X_GAIN_1X 0x00
#define AS7265X_GAIN_4X 0x01
#define AS7265X_GAIN_16X 0x02
#define AS7265X_GAIN_64X 0x03

// Device type identifiers
#define AS7265X_DEVICE_ID 0x41 // Should match for the master sensor

// Indicator LED mode
#define AS7265X_LED_WHITE 0
#define AS7265X_LED_UV 1
#define AS7265X_LED_IR 2

class AS7265X {
public:
    bool begin();
    bool isConnected();
    float getTemperature();
    
    // Measurement control
    void takeMeasurements();
    void takeMeasurementsWithBulb(uint8_t bulbType = AS7265X_LED_WHITE);
    
    // Channel data getters (all 18 channels)
    float getChannel(uint8_t channelIndex);
    
    // Individual channel getters for convenience
    float getA(); // 410nm
    float getB(); // 435nm
    float getC(); // 460nm
    float getD(); // 485nm
    float getE(); // 510nm
    float getF(); // 535nm
    float getG(); // 560nm
    float getH(); // 585nm
    float getI(); // 610nm
    float getJ(); // 645nm
    float getK(); // 680nm
    float getL(); // 705nm
    float getM(); // 730nm (alias for R)
    float getN(); // 760nm (alias for S)
    float getO(); // 810nm (alias for T)
    float getP(); // 860nm (alias for U)
    float getQ(); // 900nm (alias for V)
    float getR(); // 940nm (alias for W)
    
    // Convenience method for NIR channels (same as AS7263)
    float getR_AS7263(); // 610nm (same as I)
    float getS_AS7263(); // 680nm (same as K)
    float getT_AS7263(); // 730nm (alias for M)
    float getU_AS7263(); // 760nm (alias for N)
    float getV_AS7263(); // 810nm (alias for O)
    float getW_AS7263(); // 860nm (alias for P)
    
    // Configuration
    void setIntegrationTime(uint8_t time);
    void setGain(uint8_t gain);
    void setBulbCurrent(uint8_t current);
    void enableBulb(uint8_t type);
    void disableBulb();
    
private:
    // I2C communication with virtual registers
    uint8_t virtualReadRegister(uint8_t virtualAddr);
    void virtualWriteRegister(uint8_t virtualAddr, uint8_t value);
    
    // Device selection within the sensor module
    uint8_t bankReadRegister(uint8_t device, uint8_t reg);
    void bankWriteRegister(uint8_t device, uint8_t reg, uint8_t value);
    
    // Raw data reading
    uint16_t readRegister(uint8_t device, uint8_t addr);
    
    // Data ready and wait functions
    bool dataReady();
    void waitForDataReady();
    
    // Float conversion for channel readings
    float convertBytesToFloat(uint16_t raw);
    
    // Helper functions for LED control
    void enableIndicator();
    void disableIndicator();
};

#endif // AS7265X_H