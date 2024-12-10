#include <Arduino.h>
#include <Adafruit_BNO055.h> 
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>

#define BNO055_SAMPLERATE_DELAY_MS 10

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

int raw_deg;
int reset_deg;
int deg;

bool now_detected;
bool previous_detected;

void setup() {
  Serial.begin(9600);
  pinMode(1, INPUT_PULLDOWN);

  delay(1000);

  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  bno.setExtCrystalUse(true);
}

void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  raw_deg = euler.x();
  gyr = (int)(euler.x() - reset_deg + 360) % 360;

  Serial.print(analogRead(1));
  Serial.print(", ");
  Serial.println(deg);

  if(analogRead(1) == 1023)
  {
    now_detected = true;

    if(previous_detected == false)
    {
      reset_deg = raw_deg;
    }
  }
  else
  {
    now_detected = false;
  }

  previous_detected = now_detected;
}