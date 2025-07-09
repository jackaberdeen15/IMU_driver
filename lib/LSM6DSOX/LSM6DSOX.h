#ifndef LSM6DSOX_H
#define LSM6DSOX_H

#include <Arduino.h>

//encapsulates all funcationality for IMU communication
class LSM6DSOX
{    
public:
    bool begin(); //sets up the sensor (calls configure sensor)
    void readAccel(float &x, float &y, float &z); // gets raw sensor values and returns them as floats

private:
    uint8_t readRegister(uint8_t reg); //low level I2C operation for reading register
    void writeRegister(uint8_t reg, uint8_t value); //low level I2C operation for writing register
    void readMultiple(uint8_t startReg, uint8_t *buffer, size_t length); //Reads a block of register at once, i.e. (x,y,z)
    void configureSensor(); // 

    // register addresses 
    static const uint8_t I2C_ADDR = 0x6A;
    static const uint8_t WHO_AM_I = 0x0F;
    static const uint8_t CTRL1_XL = 0x10;
    static const uint8_t OUTX_L_XL = 0x28;
};


#endif