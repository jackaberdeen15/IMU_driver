#include "LSM9DS1_Mag.h"
#include <Wire.h>

bool LSM9DS1_Mag::begin(TwoWire &wire){
    _wire = &wire;
    Serial.println("Starting imu.begin()...");
    _wire->begin(); //start I2C communication
    delay(10);

    uint8_t whoami = readRegister(WHO_AM_I); // reads WHO_AM_I register to confirm sensor is connected
    Serial.print("WHO_AM_I: 0x"); Serial.println(whoami, HEX);

    uint8_t ctrl1 = readRegister(CTRL_REG1_M);
    Serial.print("CTRL_REG1_G: 0x"); Serial.println(ctrl1, BIN);
    uint8_t ctrl2 = readRegister(CTRL_REG3_M);
    Serial.print("CTRL_REG3_M: 0x"); Serial.println(ctrl2, BIN);

    if (whoami != 0x3D) return false; // whoami should return 0x6C else there is an error
    writeRegister(CTRL_REG1_M, 0b11100100); // ultra-high perf, 10 Hz ODR
    writeRegister(CTRL_REG3_M, 0x00); // continious conversion mode

    return true;
}


void LSM9DS1_Mag::read(float &x, float &y, float &z){
    readRaw(x,y,z);
    x -= BiasX;
    y -= BiasY;
    z -= BiasZ;

    // deadband for small fluctuations which could be caused by jitter
    if (abs(x)<0.02) x=0;
    if (abs(y)<0.02) y=0;
    if (abs(z)<0.02) z=0;
}

void LSM9DS1_Mag::readRaw(float &x, float &y, float &z){
    uint8_t buffer[6];
    _wire->beginTransmission(I2C_ADDR);
    _wire->write(OUT_X_L_M);
    _wire->endTransmission(false);
    _wire->requestFrom(I2C_ADDR,6);

    for (int i = 0; i<6; ++i){
        buffer[i] = _wire->read();
    }

    //combine LSB and MSB into 16-bit integers
    int16_t rawX = (int16_t)(buffer[1] << 8 | buffer[0]);
    int16_t rawY = (int16_t)(buffer[3] << 8 | buffer[2]);
    int16_t rawZ = (int16_t)(buffer[5] << 8 | buffer[4]);

    //
    const float sensitivity = 0.00014f ; // scaling for 0.14 mgauss/lsb 
    x = rawX * sensitivity;
    y = rawY * sensitivity;
    z = rawZ * sensitivity;
}

void LSM9DS1_Mag::calibrate(int samples){
    float sumX = 0, sumY = 0, sumZ = 0;

    Serial.println("Calibrating Magnetometer... Please keep the board still!");

    for (int i=0; i<samples; i++){
        float ax, ay, az;
        readRaw(ax,ay,az); //function reads values from sensor
        sumX+=ax;
        sumY+=ay;
        sumZ+=az;
        delay(10); //sample at 100Hz
    }

    BiasX = sumX / samples;
    BiasY = sumY / samples;
    BiasZ = sumZ / samples; //subtract gravity (in g)

    Serial.println("Calibration complete.");
    Serial.print("Bias X: "); Serial.println(BiasX);
    Serial.print("Bias Y: "); Serial.println(BiasY);
    Serial.print("Bias Z: "); Serial.println(BiasZ);
}

uint8_t LSM9DS1_Mag::readRegister(uint8_t reg) {
    _wire->beginTransmission(I2C_ADDR); //start communication with sensor
    _wire->write(reg); // tell sensor which register we want to read
    _wire->endTransmission(false); //send a repeated start (dont release the bus)
    _wire->requestFrom(I2C_ADDR, (uint8_t)1); // request 1 byte from that register
    return _wire->read(); //Read and return the byte
}

void LSM9DS1_Mag::writeRegister(uint8_t reg, uint8_t value) {
    _wire->beginTransmission(I2C_ADDR); //begin communication
    _wire->write(reg); // register to write to
    _wire->write(value); // value to write
    _wire->endTransmission(); //complete transmission
}
