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
  } else {
    Serial.println("IMU initialisation failed.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  float ax, ay, az;
  imu.readAccel(ax, ay, az);


  
  Serial.print("Accel X: ");
  Serial.print(ax);
  Serial.print(", Y: ");
  Serial.print(ay);
  Serial.print(", Z: ");
  Serial.print(az);
  Serial.println();
  delay(500);
}



