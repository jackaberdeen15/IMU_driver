#ifndef LSM9DS1_H
#define LSM9DS1_H

#include <Arduino.h>
#include <Wire.h>
#include <mbed.h>


#include "LSM9DS1_Accel.h"
#include "LSM9DS1_Gyro.h"
#include "LSM9DS1_Mag.h"

struct IMUBias{
  float accelBiasX, accelBiasY, accelBiasZ;
  float gyroBiasX, gyroBiasY, gyroBiasZ;
  uint32_t magic;
};

class LSM9DS1 {
public:
  LSM9DS1(TwoWire &wire = Wire1): _wire(&wire){}

  bool begin(bool autoCalibrateIfNeeded = true);
  

  void calibrateAll();
  void resetCalibration();

  void readAll(float &ax, float &ay, float &az,
               float &gx, float &gy, float &gz,
               float &mx, float &my, float &mz);

  LSM9DS1_Accel accel;
  LSM9DS1_Gyro gyro;
  LSM9DS1_Mag mag;

private:
  TwoWire* _wire;
  IMUBias biasData;

  // Flash storage helpers
    bool loadCalibration();
    void saveCalibration();
    void eraseCalibration();

    // FlashIAP instance and flash parameters
    mbed::FlashIAP flash;
    static constexpr uint32_t CALIB_MAGIC = 0xDEADBEEF;
    uint32_t flash_start;
};

#endif