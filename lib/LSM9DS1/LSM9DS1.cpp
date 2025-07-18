#include "LSM9DS1.h"

bool LSM9DS1::begin(bool autoCalibrateIfNeeded){
    
    flash.init();
    flash_start = flash.get_flash_start() + flash.get_flash_size() / 2;

    if(!accel.begin() || !gyro.begin() || !mag.begin()){
        Serial.println("Failed to initialise one or more sensors.");
        return false;
    }
    Serial.println("Successfully initialised all sensors.");

    if (autoCalibrateIfNeeded && loadCalibration()){
        Serial.println("Loading calibration...");
        accel.setBias(biasData.accelBiasX, biasData.accelBiasY, biasData.accelBiasZ);
        gyro.setBias(biasData.gyroBiasX, biasData.gyroBiasY, biasData.gyroBiasZ);
        Serial.println("Loaded calibration from flash.");
    } else if (autoCalibrateIfNeeded){
        Serial.println("No valid calibration found, running calibration...");
        calibrateAll();
        Serial.println("Finished calibration.");
    }
    Serial.println("Got here");
    return true;
}

void LSM9DS1::calibrateAll() {
    Serial.println("Calibrating accelerometer and gyroscope. Keep still...");

    accel.calibrate();
    gyro.calibrate();

    accel.getBias(biasData.accelBiasX, biasData.accelBiasY, biasData.accelBiasZ);
    gyro.getBias(biasData.gyroBiasX, biasData.gyroBiasY, biasData.gyroBiasZ);
    biasData.magic = CALIB_MAGIC;

    saveCalibration();
    Serial.println("Calibration complete and saved.");
}

void LSM9DS1::resetCalibration(){
    eraseCalibration();
    Serial.println("Calibration reset. Will recalibrate next startup.");
}

void LSM9DS1::readAll(float &ax, float &ay, float &az,
                      float &gx, float &gy, float &gz,
                      float &mx, float &my, float &mz) {
    accel.read(ax, ay, az);
    gyro.read(gx, gy, gz);
    mag.read(mx, my, mz);
}

bool LSM9DS1::loadCalibration(){
    Serial.println("Fetching calibration values from flash...");
    IMUBias temp;
    flash.read(&temp, flash_start, sizeof(IMUBias));
    if (temp.magic == CALIB_MAGIC) {
        Serial.println("Calibration values fetched from flash.");
        biasData = temp;
        return true;
    }
    return false;
}

void LSM9DS1::saveCalibration(){
    flash.erase(flash_start, flash.get_page_size());
    flash.program((const void*)&biasData, flash_start, sizeof(IMUBias));
}

void LSM9DS1::eraseCalibration() {
    flash.erase(flash_start, flash.get_page_size());
}