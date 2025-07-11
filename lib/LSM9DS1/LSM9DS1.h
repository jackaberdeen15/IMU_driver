#ifndef LSM9DS1_H
#define LSM9DS1_H

#include "LSM9DS1_Accel.h"
#include "LSM9DS1_Gyro.h"
#include "LSM9DS1_Mag.h"

class LSM9DS1 {
public:
  LSM9DS1(TwoWire &wire = Wire1):  _wire(&wire) {}

  bool begin() {
    return accel.begin(*_wire) && gyro.begin(*_wire) && mag.begin(*_wire);
  }

  void calibrateAll() {
    accel.calibrate();
    gyro.calibrate();
    mag.calibrate();
  }

  void readAll(float &ax, float &ay, float &az,
               float &gx, float &gy, float &gz,
               float &mx, float &my, float &mz) {
    accel.read(ax, ay, az);
    gyro.read(gx, gy, gz);
    mag.read(mx, my, mz);
  }

  LSM9DS1_Accel accel;
  LSM9DS1_Gyro gyro;
  LSM9DS1_Mag mag;

private:
  TwoWire* _wire;
};

#endif