#include <Arduino.h>

const int pin[6] = {3, 2, 1, 0, 5, 4};
int value[6];

bool is_detected_any_sensor()
{
  for(int i = 0; i < 6; i++)
  {
    if(value[i] == 0)
    {
      return true;
    }
  }

  return false;

}

void setup() {

  for(int i = 0; i < 6; i++)
  {
    pinMode(pin[i], INPUT_PULLUP);
  }
}

void loop() {
  for(int i = 0; i < 6; i++)
  {
    value[i] = digitalRead(pin[i]);
  }

  if(is_detected_any_sensor())
  {
    analogWrite(6, 1023);
  }
  else
  {
    analogWrite(6, 0);
  }
}