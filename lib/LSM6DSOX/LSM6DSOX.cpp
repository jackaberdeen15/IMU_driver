#include "LSM6DSOX.h"
#include "Wire.h" // brings I2C library

uint8_t LSM6DSOX::readRegister(uint8_t reg) {
    Wire.beginTransmission(I2C_ADDR); //start communication with sensor
    Wire.write(reg); // tell sensor which register we want to read
    Wire.endTransmission(false); //send a repeated start (dont release the bus)
    Wire.requestFrom(I2C_ADDR, (uint8_t)1); // request 1 byte from that register
    return Wire.read(); //Read and return the byte
}

void LSM6DSOX::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(I2C_ADDR); //begin communication
    Wire.write(reg); // register to write to
    Wire.write(value); // value to write
    Wire.endTransmission(); //complete transmission
}

void LSM6DSOX::readMultiple(uint8_t startReg, uint8_t *buffer, size_t length) {
    Wire.beginTransmission(I2C_ADDR); //begin transmission
    Wire.write(startReg); //starting register (e.g. OUTX_L_XL)
    Wire.endTransmission(false); //repeated start

    Wire.requestFrom(I2C_ADDR, (uint8_t)length); //request N bytes
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = Wire.read(); //store each byte in buffer
    }
}

bool LSM6DSOX::begin(){

    Wire.begin(); //start I2C communication
    uint8_t whoami = readRegister(WHO_AM_I); // reads WHO_AM_I register to confirm sensor is connected
    Serial.print("WHO_AM_I: 0x"); Serial.println(whoami, HEX);
    if (whoami != 0x6C) return false; // whoami should return 0x6C else there is an error
    configureSensor();

    return true;
}

void LSM6DSOX::configureSensor(){
    // 104 Hz, 2g, BW = 50 Hz (from datasheet)
    writeRegister(CTRL1_XL, 0b01000000); //write binary value to CTRL1_XL to enable the accelerometer at 104 Hz output datarate, +/- 2g range, with default bandwidth filter
}

void LSM6DSOX::readAccel(float &x, float &y, float &z){
    uint8_t buffer[6];
    readMultiple(OUTX_L_XL, buffer, 6); //stores x_l, x_h, y_l, y_h, z_l, z_h

    //combine LSB and MSB into 16-bit integers
    int16_t rawX = (int16_t)(buffer[1] << 8 | buffer[0]);
    int16_t rawY = (int16_t)(buffer[3] << 8 | buffer[2]);
    int16_t rawZ = (int16_t)(buffer[5] << 8 | buffer[4]);

    //convert raw lsb values to g-forces using sensitivity for +/- 2g mode (divided by 1000 as 0.061 mg/LSB is converted into 0.000061 g/LSB)
    const float sensitivity = 0.061f; // mg/lsb for +/- 2g 
    x = rawX * sensitivity / 1000.0f;
    y = rawY * sensitivity / 1000.0f;
    z = rawZ * sensitivity / 1000.0f;
}


