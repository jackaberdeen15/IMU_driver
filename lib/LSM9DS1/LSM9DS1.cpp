#include "LSM9DS1.h"
#include "Wire.h" // brings I2C library

uint8_t LSM9DS1::readRegister(uint8_t reg) {
    Wire1.beginTransmission(I2C_ADDR); //start communication with sensor
    Wire1.write(reg); // tell sensor which register we want to read
    Wire1.endTransmission(false); //send a repeated start (dont release the bus)
    Wire1.requestFrom(I2C_ADDR, (uint8_t)1); // request 1 byte from that register
    return Wire1.read(); //Read and return the byte
}

void LSM9DS1::writeRegister(uint8_t reg, uint8_t value) {
    Wire1.beginTransmission(I2C_ADDR); //begin communication
    Wire1.write(reg); // register to write to
    Wire1.write(value); // value to write
    Wire1.endTransmission(); //complete transmission
}

void LSM9DS1::readMultiple(uint8_t startReg, uint8_t *buffer, size_t length) {
    Wire1.beginTransmission(I2C_ADDR); //begin transmission
    Wire1.write(startReg); //starting register (e.g. OUTX_L_XL)
    Wire1.endTransmission(false); //repeated start

    Wire1.requestFrom(I2C_ADDR, (uint8_t)length); //request N bytes
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = Wire1.read(); //store each byte in buffer
    }
}

void LSM9DS1::scanI2C() {
  Serial.println("Scanning I2C...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire1.beginTransmission(addr);
    if (Wire1.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(addr, HEX);
    }
  }
}

bool LSM9DS1::begin(){
    Serial.println("Starting imu.begin()...");
    Wire1.begin(); //start I2C communication
    delay(10);
    Serial.println("Starting I2C scan...");
    scanI2C();

    uint8_t whoami = readRegister(WHO_AM_I); // reads WHO_AM_I register to confirm sensor is connected
    Serial.print("WHO_AM_I: 0x"); Serial.println(whoami, HEX);

    uint8_t ctrl1 = readRegister(CTRL_REG6_XL);
    Serial.print("CTRL_REG6_XL: 0x"); Serial.println(ctrl1, BIN);

    if (whoami != 0x68) return false; // whoami should return 0x6C else there is an error
    configureSensor();

    return true;
}

void LSM9DS1::configureSensor(){
    // 104 Hz, 2g, BW = 50 Hz (from datasheet)
    writeRegister(CTRL_REG6_XL, 0b01100000); //write binary value to CTRL1_XL to enable the accelerometer at 119 Hz output datarate, +/- 2g range, with default bandwidth filter
    calibrateAccelBias(500); // calibrate the sensor for bias

    uint8_t reg = readRegister(CTRL_REG6_XL);
    Serial.print("CTRL_REG6_XL = 0x");
    Serial.println(reg, HEX);
}

void LSM9DS1::readAccel(float &x, float &y, float &z){
    readRawAccel(x,y,z);
    x -= accelBiasX;
    y -= accelBiasY;
    z -= accelBiasZ;

    // deadband for small fluctuations which could be caused by jitter
    if (abs(x)<0.02) x=0;
    if (abs(y)<0.02) y=0;
    if (abs(z)<0.02) z=0;
}

void LSM9DS1::readRawAccel(float &x, float &y, float &z){
    uint8_t buffer[6];
    readMultiple(OUTX_L_XL, buffer, 6); //stores x_l, x_h, y_l, y_h, z_l, z_h

    //combine LSB and MSB into 16-bit integers
    int16_t rawX = (int16_t)(buffer[1] << 8 | buffer[0]);
    int16_t rawY = (int16_t)(buffer[3] << 8 | buffer[2]);
    int16_t rawZ = (int16_t)(buffer[5] << 8 | buffer[4]);

    //convert raw lsb values to g-forces using sensitivity for +/- 2g mode (divided by 1000 as 0.061 mg/LSB is converted into 0.000061 g/LSB)
    const float sensitivity = 0.061f / 1000.0f; // g/lsb for +/- 2g 
    x = rawX * sensitivity;
    y = rawY * sensitivity;
    z = rawZ * sensitivity;
}

void LSM9DS1::calibrateAccelBias(int samples){
    float sumX = 0, sumY = 0, sumZ = 0;

    Serial.println("Calibrating sensor... Please keep the board still!");

    for (int i=0; i<samples; i++){
        float ax, ay, az;
        readRawAccel(ax,ay,az); //function reads values from sensor
        sumX+=ax;
        sumY+=ay;
        sumZ+=az;
        delay(10); //sample at 100Hz
    }

    accelBiasX = sumX / samples;
    accelBiasY = sumY / samples;
    accelBiasZ = sumZ / samples; //subtract gravity (in g)

    Serial.println("Calibration complete.");
    Serial.print("Bias X: "); Serial.println(accelBiasX);
    Serial.print("Bias Y: "); Serial.println(accelBiasY);
    Serial.print("Bias Z: "); Serial.println(accelBiasZ);
}


