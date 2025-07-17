#include <Arduino.h>
#include "LSM9DS1.h"
// put function declarations here:
LSM9DS1 imu;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!imu.begin()) {
    Serial.println("IMU failed to initialize.");
    while (1);
  }

  Serial.println("IMU ready.");
}

void loop() {
  float ax, ay, az, gx, gy, gz, mx, my, mz;
  imu.readAll(ax, ay, az, gx, gy, gz, mx, my, mz);

  Serial.print("Accel X: "); Serial.print(ax);
  Serial.print(" Y: "); Serial.print(ay);
  Serial.print(" Z: "); Serial.println(az);

  delay(500);

  // Manual recalibration if 'C' is typed in Serial Monitor
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'C' || c == 'c') {
      imu.calibrateAll();
    }
  }
}



