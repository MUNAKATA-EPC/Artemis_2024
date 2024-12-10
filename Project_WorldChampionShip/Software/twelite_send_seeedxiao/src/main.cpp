#include <Arduino.h>

char send_value = 'a';

bool previous_but;
bool now_but;

void setup() {
  Serial.begin(9600);

  Serial1.begin(115200);

  pinMode(1, INPUT_PULLDOWN);
}

void loop() {
  if(analogRead(1) == 1023)
  {
    now_but = true;

    if(previous_but == false)
    {
      digitalWrite(LED_BUILTIN, LOW);

      if(send_value == 'a')
      {
        send_value = 'b';
      }
      else if(send_value == 'b')
      {
        send_value = 'c';
      }
      else if(send_value == 'c')
      {
        send_value = 'a';
      }
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
  else
  {
    now_but = false;
    digitalWrite(LED_BUILTIN, HIGH);
  }

  Serial1.write(send_value);
  Serial1.write('\n');
  Serial1.flush();

  previous_but = now_but;

  delay(100);
}