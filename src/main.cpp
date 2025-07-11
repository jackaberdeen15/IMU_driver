#include <Arduino.h>
#include "LSM9DS1.h"
// put function declarations here:
LSM9DS1 imu;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);  // <-- wait for Serial Monitor to connect (important!)
  Serial.println("Setup started.");
  if (imu.begin()){
    Serial.println("IMU initialised successfully");
    Serial.println("Calibrating sensors now.");
    imu.calibrateAll();
  } else {
    Serial.println("IMU initialisation failed.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  float ax, ay, az, gx, gy, gz, mx, my, mz;
  imu.readAll(ax, ay, az, gx, gy, gz, mx, my, mz);
  char string[64];
  sprintf(string, "Accel X: %.2f, Y: %.2f, Z: %.2f", (ax, ay, az));
  Serial.println(string);
  sprintf(string, "Gyro X: %.2f, Y: %.2f, Z: %.2f", (gx, gy, gz));
  Serial.println(string);
  sprintf(string, "Mag X: %.2f, Y: %.2f, Z: %.2f", (mx, my, mz));
  Serial.println(string);

  delay(500);
}



