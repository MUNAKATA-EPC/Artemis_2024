#include <Arduino.h>

int pinOut[] = {0, 1, 2, 3, 4, 5, 8, 9};
float Line_deg;

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 8; i++)
  {
    pinMode(pinOut[i], INPUT_PULLUP);
  }

  Serial1.begin(19200);
}

void loop() {
  String str;

  for(int i = 0; i < 8; i++)
  {
    //Serial.print(digitalRead(pinOut[i]));
    //erial.print(", ");
    str += String(digitalRead(pinOut[i]));
  }

    char line[8] = {str[0],str[1],str[2],str[3],str[4],str[5],str[6],str[7]};
    if(line[0] == '0')
  {
    if(line[1] == '0')
    {
      if(line[2] == '0')
      {
        Line_deg = 67.5;
      }
      else if(line[3] == '0')
      {
        Line_deg = 90;
      }
      else if(line[4] == '0')
      {
        Line_deg = 292.5;
      }
      else if(line[5] == '0')
      {
        Line_deg = 315;
      }
      else if(line[6] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 360;
      }
      else
      {
        Line_deg = 45;
      }
    } 
    else if(line[2] == '0')
    {
      if(line[3] == '0')
      {
        Line_deg = 90;
      }
      else if(line[4] == '0')
      {
        Line_deg = 292.5;
      }
      else if(line[5] == '0')
      {
        Line_deg = 315;
      }
      else if(line[6] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 360;
      }
      else
      {
        Line_deg = 67.5;
      }
    }
    else if(line[3] == '0')
    {
      if(line[4] == '0')
      {
        Line_deg = 292.5;
      }
      else if(line[5] == '0')
      {
        Line_deg = 315;
      }
      else if(line[6] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 360;
      }
      else
      {
        Line_deg = 90;
      }
    }
    else if(line[4] == '0')
    {
      if(line[5] == '0')
      {
        Line_deg = 315;
      }
      else if(line[6] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 360;
      }
      else
      {
        Line_deg = 292.5;
      }
    }
    else if(line[5] == '0')
    {
      if(line[6] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 360;
      }
      else
      {
        Line_deg = 315;
      }
    }
    else if(line[6] == '0')
    {
      if(line[7] == '0')
      {
        Line_deg = 360;
      }
      else
      {
        Line_deg = 337.5;
      }
    }
    else if(line[7] == '0')
    {
      Line_deg = 360;
    }
    else
    {
      Line_deg = 22.5;
    }
  }
  else if(line[1] == '0')
  {
    if(line[2] == '0')
    {
      if(line[3] == '0')
      {
        Line_deg = 112.5;
      }
      else if(line[4] == '0')
      {
        Line_deg = 135;
      }
      else if(line[5] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[6] == '0')
      {
        Line_deg = 360;
      }
      else if(line[7] == '0')
      {
        Line_deg = 22.5;
      }
      else
      {
        Line_deg = 45;
      }
    }
    else if(line[3] == '0')
    {
      if(line[4] == '0')
      {
        Line_deg = 135;
      }
      else if(line[5] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[6] == '0')
      {
        Line_deg = 360;
      }
      else if(line[7] == '0')
      {
        Line_deg = 22.5;
      }
      else
      {
        Line_deg = 112.5;
      }
    }
    else if(line[4] == '0')
    {
      if(line[5] == '0')
      {
        Line_deg = 337.5;
      }
      else if(line[6] == '0')
      {
        Line_deg = 360;
      }
      else if(line[7] == '0')
      {
        Line_deg = 22.5;
      }
      else
      {
        Line_deg = 135;
      }
    }
    else if(line[5] == '0')
    {
      if(line[6] == '0')
      {
        Line_deg = 360;
      }
      else if(line[7] == '0')
      {
        Line_deg = 22.5;
      }
      else
      {
        Line_deg = 337.5;
      }   
    }
    else if(line[6] == '0')
    {
      if(line[7] == '0')
      {
        Line_deg = 22.5;
      }
      else
      {
        Line_deg = 360;
      }
    }
    else if(line[7] == '0')
    {
      Line_deg = 22.5;
    }
    else
    {
      Line_deg = 67.5;
    }
  }
  else if(line[2] == '0')
  {
    if(line[3] == '0')
    {
      if(line[4] == '0')
      {
        Line_deg = 157.5;
      }
      else if(line[5] == '0')
      {
        Line_deg = 180;
      }
      else if(line[6] == '0')
      {
        Line_deg = 22.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 45;
      }
      else
      {
        Line_deg = 135;
      }
    }
    else if(line[4] == '0')
    {
      if(line[5] == '0')
      {
        Line_deg = 180;
      }
      else if(line[6] == '0')
      {
        Line_deg = 22.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 45;
      }
      else
      {
        Line_deg = 157.5;
      }
    }
    else if(line[5] == '0')
    {
      if(line[6] == '0')
      {
        Line_deg = 22.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 45;
      }
      else
      {
        Line_deg = 180;
      }   
    }
    else if(line[6] == '0')
    {
      if(line[7] == '0')
      {
        Line_deg = 45;
      }
      else
      {
        Line_deg = 22.5;
      }
    }
    else if(line[7] == '0')
    {
      Line_deg = 45;
    }
    else
    {
      Line_deg = 112.5;
    }
  }
  else if(line[3] == '0')
  {
    if(line[4] == '0')
    {
      if(line[5] == '0')
      {
        Line_deg = 202.5;
      }
      else if(line[6] == '0')
      {
        Line_deg = 225;
      }
      else if(line[7] == '0')
      {
        Line_deg = 67.5;
      }
      else
      {
        Line_deg = 180;
      }
    }
    else if(line[5] == '0')
    {
      if(line[6] == '0')
      {
        Line_deg = 225;
      }
      else if(line[7] == '0')
      {
        Line_deg = 67.5;
      }
      else
      {
        Line_deg = 202.5;
      }
    }
    else if(line[6] == '0')
    {
      if(line[7] == '0')
      {
        Line_deg = 67.5;
      }
      else
      {
        Line_deg = 225;
      }
    }
    else if(line[7] == '0')
    {
      Line_deg = 67.5;
    }
    else
    {
      Line_deg = 157.5;
    }
  }
  else if(line[4] == '0')
  {
    if(line[5] == '0')
    {
      if(line[6] == '0')
      {
        Line_deg = 247.5;
      }
      else if(line[7] == '0')
      {
        Line_deg = 270;
      }
      else
      {
        Line_deg = 225;
      }
    }
    else if(line[6] == '0')
    {
      if(line[7] == '0')
      {
        Line_deg = 270;
      }
      else
      {
        Line_deg = 247.5;
      }
      
    }
    else if(line[7] == '0')
    {
      Line_deg = 270;
    }
    else
    {
      Line_deg = 202.5;
    }
  }
  else if(line[5] == '0')
  {
    if(line[6] == '0')
    {
      if(line[7] == '0')
      {
        Line_deg = 292.5;
      }
      else
      {
        Line_deg =270;
      }
    }
    else if(line[7] == '0')
    {
      Line_deg = 292.5;
    }
    else
    {
      Line_deg = 247.5;
    }
  }
  else if(line[6] == '0')
  {
    if(line[7] == '0')
    {
      Line_deg = 315;
    }
    else
    {
      Line_deg = 292.5;
    }
  }
  else if(line[7] == '0')
  {
    Line_deg = 337.5;
  }
  else
  {
    Line_deg = 512;
  }
 

  Serial1.print(Line_deg);
  Serial1.print('a');
  Serial1.print(String("\t"));
  


  delay(10);

}