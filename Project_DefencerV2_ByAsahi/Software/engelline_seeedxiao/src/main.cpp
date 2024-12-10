#include <Arduino.h>
#include "digitalWriteFast.h"
#include <iostream>
#include <string>
#include <string>

int line_pins[8] = { 3, 2, 1, 0, 9, 8, 5, 4};
int line_value[8] = {0, 0, 0, 0, 0, 0, 0, 0};

float line_sensor_degs[8] = {0, 45, 90, 135, 180, 225, 270, 315};

int line_deg;
int line_distance;
int before_line_deg;


//指定されたピン番号のS4282-51の値を読み取ります。
void GetSensorValue()
{
  for(int i = 0; i < 8; i++)
  {
    line_value[i] = analogRead(line_pins[i]);
    line_value[i] = line_value[i] > 30 ? 1 : 0;
  }
}

//反応しているセンサーの個数を返します。
int GetDetectedSensorCount(){
  int ret = 0;
  for(int i = 0; i < 8; i++)
  {
    if(line_value[i] == 0)
    {
      ret++;
    }
  }

  return ret;
}

void setup() {
  for(int i = 0; i < 8; i++)
  {
    pinMode(line_pins[i], INPUT_PULLUP);
  }

  Serial.begin(9600);
  Serial1.begin(115200);
}

void loop() {
  GetSensorValue();
  if(GetDetectedSensorCount() == 0)
  {
    //反応したセンサーがない
    line_deg = -1;
    line_distance = -1;
  }
  else
  {
    if(GetDetectedSensorCount() == 1)
    {
      for(int i = 0; i < 8; i++)
      {
        if(line_value[i] == 0)
        {
          line_deg = i * 45;
          line_distance = 0;

          break;
        }
      }

      line_deg = (line_deg + 45 + 1) % 360;
    }
    else
    { 
      float _line_vecx = 0;
      float _line_vecy = 0;

      float _line_deg = 0;

      for(int i = 0; i < 8; i++)
      {
        if(line_value[i] == 0)
        {
          _line_vecx += cos(radians(line_sensor_degs[i]));
          _line_vecy += sin(radians(line_sensor_degs[i]));
        }
      }

      _line_deg = degrees(atan2(_line_vecy, _line_vecx));
      _line_deg = _line_deg <= 0 ? _line_deg + 360 : _line_deg;
      line_deg = (int)(_line_deg + 22.5) % 360;



      if(_line_vecx == 0 && _line_vecy == 0)
      {
        line_deg = before_line_deg;
      }

      int line_distance_mode = 0; 
      int line_dis_temp[2] = {0, 0};
      for(int i = 0; i < 8; i++)
      {
        if(line_value[i] == 0)
        {
          if(line_value[(i - 1 + 8) % 8] == 0)
          {
            if(GetDetectedSensorCount() == 2)
            {
              line_dis_temp[0] = 1;
              line_dis_temp[1] = 5;
            }
            //スルー、何もしない。
          }
          else
          {
            line_dis_temp[line_distance_mode]++;
          }
        }
        else
        {
          if(line_value[(i - 1 + 8) % 8] == 0)
          {
            line_distance_mode = (line_distance_mode + 1) % 2;
            line_dis_temp[line_distance_mode]++;
          }
          else
          {
            line_dis_temp[line_distance_mode]++;
          }
        }
      }

      if(line_dis_temp[0] > line_dis_temp[1])
      {
        line_distance = line_dis_temp[1];
      }
      else
      {
        line_distance = line_dis_temp[0];
      }
    }
  }

  Serial1.print(line_deg);
  Serial1.print('a');
  Serial1.print("\t");
  Serial1.print(line_distance);
  Serial1.print('b');
  Serial1.print("\t");
  Serial1.flush();

  before_line_deg = line_deg;

  delay(10);
}