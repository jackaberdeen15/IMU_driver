#include "LSM9DS1.h"

bool LSM9DS1::begin(bool autoCalibrateIfNeeded){
    if(!accel.begin() || !gyro.begin() || !mag.begin()){
        Serial.println("Failed to initialise one or more sensors.");
        return false;
    }

    if (autoCalibrateIfNeeded && loadCalibration()){
        accel.setBias(biasData.accelBiasX, biasData.accelBiasY, biasData.accelBiasZ);
        gyro.setBias(biasData.gyroBiasX, biasData.gyroBiasY, biasData.gyroBiasZ);
        Serial.println("Loaded calibration from flash.");
    } else if (autoCalibrateIfNeeded){
        Serial.println("No valid calibration found, running calibration...");
        calibrateAll();
    }

    return true;
}

void LSM9DS1::resetCalibration(){
    biasData.magic = 0; //invalidate it
    saveCalibration();
    Serial.println("Calibration reset. Will recalibrate next startup");
}

void LSM9DS1::readAll(float &ax, float &ay, float &az,
                      float &gx, float &gy, float &gz,
                      float &mx, float &my, float &mz) {
  accel.read(ax, ay, az);
  gyro.read(gx, gy, gz);
  mag.read(mx, my, mz);
}

bool LSM9DS1::loadCalibration(){
    uint8_t *ptr = (uint8_t *)&biasData;
  for (size_t i = 0; i < sizeof(IMUBias); i++) {
    ptr[i] = EEPROM.read(i);
  }
  return (biasData.magic == CALIB_MAGIC);
}

void LSM9DS1::saveCalibration(){
    uint8_t *ptr = (uint8_t *)&biasData;
    for (size_t i = 0; i < sizeof(IMUBias); i++) {
        EEPROM.update(i, ptr[i]);  // update() writes only if changed (reduces flash wear)
    }
}