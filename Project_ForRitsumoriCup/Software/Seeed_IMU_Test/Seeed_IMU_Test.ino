#include "LSM6DS3.h"
#include "Wire.h"

//Create a instance of class LSM6DS3
LSM6DS3 IMU(I2C_MODE, 0x6A);    //I2C device address 0x6A

float IMU_angle;

float Cal_Value = 1.8;

void CalcAngle(bool bcal)
{
    IMU_angle += IMU.readFloatGyroZ() + (bcal ? 0 : Cal_Value);
}

void Calibration()
{
  Resetangle();

  for(int i = 0; i < 1000 / 5; i++)
  {
    CalcAngle(true);
    delay(5);
  }

  Cal_Value = IMU_angle / (1000 / 5);

  Resetangle();
}

void Resetangle()
{
  IMU_angle = 0;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    Calibration();
}

void loop() {
    CalcAngle(false);

    Serial.println(IMU_angle);

    delay(5);
}
