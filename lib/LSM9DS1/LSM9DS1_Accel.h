#ifndef LSM9DS1_Accel_H
#define LSM9DS1_Accel_H

#include <Arduino.h>
#include <Wire.h>

//encapsulates all funcationality for IMU communication
class LSM9DS1_Accel
{    
public:
    bool begin(TwoWire &wire = Wire1); //sets up the sensor (calls configure sensor)
    void read(float &x, float &y, float &z); // gets calibrated sensor values and returns them as floats
    void calibrate(int samples = 100); //calibrate the offset bias of the sensor while at rest
private:
    TwoWire* _wire;
    
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void readRaw(float &x, float &y, float &z);

    // register addresses 
    static const uint8_t I2C_ADDR = 0x6B;
    static const uint8_t WHO_AM_I = 0x0F;
    static const uint8_t CTRL_REG6_XL = 0x20; //control reg for Accel
    static const uint8_t OUT_X_L_XL = 0x28; //output control reg for Accel

    //offset values
    float BiasX = 0.0;
    float BiasY = 0.0;
    float BiasZ = 0.0;
};


#endif