#include <Arduino.h>

String data = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
}

void loop() {

  if(Serial1.available() > 0)
  {
    char temp = Serial1.read();

    if(temp == 'a' || temp == 'b' || temp == 'c')
    {
      data = String(temp);
    }
  }

  Serial.print(data);
  Serial.print(", ");
  Serial.println(Serial1.available());

  delay(10);
}