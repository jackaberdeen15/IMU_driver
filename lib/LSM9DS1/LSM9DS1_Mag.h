#ifndef LSM9DS1_MAG_H
#define LSM9DS1_MAG_H

#include <Arduino.h>
#include <Wire.h>

class LSM9DS1_Mag {
public:
    bool begin(TwoWire &wire = Wire1); //sets up the sensor (calls configure sensor)
    void read(float &x, float &y, float &z); // gets calibrated sensor values and returns them as floats
    void calibrate(int durationms = 10000); //calibrate the offset bias of the sensor while moving

    void setBias(float bx, float by, float bz) {
        BiasX = bx; BiasY = by; BiasZ = bz;
    }

    void getBias(float &bx, float &by, float &bz) {
        bx = BiasX; by = BiasY; bz = BiasZ;
    }

private:
    TwoWire* _wire;
    
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void readRaw(float &x, float &y, float &z);

    // register addresses 
    static const uint8_t I2C_ADDR = 0x1E;
    static const uint8_t WHO_AM_I = 0x0F;
    static const uint8_t CTRL_REG1_M = 0x20; //control reg 1 for Mag
    static const uint8_t CTRL_REG3_M = 0x22; //control reg 2 for Mag
    static const uint8_t OUT_X_L_M = 0x28; //output control reg for Mag

    //offset values
    float BiasX = 0.0;
    float BiasY = 0.0;
    float BiasZ = 0.0;
};

#endif