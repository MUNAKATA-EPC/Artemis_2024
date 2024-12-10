#ifndef _SERIAL_RECEIVE_HPP
#define _SERIAL_RECEIVE_HPP

#include <Arduino.h>
#include <Adafruit_BNO055.h>

#include "serial_receive.hpp"
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire2);
int Gyro_Deg;

float Ball_Deg = -1;
float Ball_Distance = -1;
float Ball_Distance_ = -1;

int Line_Deg = -1;
int Line_right = -1;
int Line_left = -1;
int Line_back = -1;
int line_min;
int line_max;

String wireless = " ";

int line_value;
int line_data[19];
int line_side_value[3];


float line_sensor_degs[16] = {0, 22.5 * 1, 22.5 * 2, 22.5 * 3, 22.5 * 4, 22.5 * 5, 22.5 * 6, 22.5 * 7, 22.5 * 0, 22.5 * 9, 22.5 * 10, 22.5 * 11, 22.5 * 12, 22.5 * 13, 22.5 * 14, 22.5 * 15};


int Yellow_Goal_Deg;
int Yellow_Goal_Distance;
int Blue_Goal_Deg;
int Blue_Goal_Distance;
int Court_Deg;
int Court_Distance;
int HoldSensor;

int raw_deg;
int reset_deg;

int robot_mode;


void serials_init()
{
    Serial.begin(9600);
    
    Serial2.begin(115200);
    
    Serial3.begin(38400);
    Serial3.setTimeout(10);

    Serial4.begin(115200);
    Serial4.setTimeout(10);

    Serial5.begin(115200);
    Serial5.setTimeout(10);

    pinMode(10, INPUT_PULLDOWN);
    pinMode(34, INPUT_PULLUP);
    pinMode(11, INPUT_PULLDOWN);
    pinMode(12, INPUT_PULLDOWN);
    
    
    if(!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while(1);
    }
    bno.setExtCrystalUse(true);
    
}

void serials_receive()
{
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

    raw_deg = euler.x();
    Gyro_Deg = (int)(euler.x() - reset_deg + 360) % 360;
    Gyro_Deg = (Gyro_Deg + 180) % 360;

    if(digitalRead(10) == HIGH)
    {
        reset_deg = raw_deg;

    }

    
    while(Serial2.available() > 0)
    {
        char _wireless = Serial2.read();

        if(_wireless == 'a')
        {
          robot_mode = 0;
        }
        else if(_wireless == 'd')
        {
          robot_mode = 1;
        }
    }

    if(Serial3.available() > 0)
    {
        Ball_Deg = Serial3.readStringUntil('a').toInt();
        Ball_Distance_ = Serial3.readStringUntil('b').toInt();
    }
    
      if(Ball_Distance_ != -1.00)
      {
          Ball_Distance = 120 - Ball_Distance_;
      }
      else
      {
        Ball_Distance = -1.00;
      }

    if(Serial4.available() > 0)
    {
        Yellow_Goal_Deg = Serial4.readStringUntil('a').toInt();
        Yellow_Goal_Distance = Serial4.readStringUntil('b').toInt();
        Blue_Goal_Deg = Serial4.readStringUntil('c').toInt();
        Blue_Goal_Distance = Serial4.readStringUntil('d').toInt();
        Court_Deg = Serial4.readStringUntil('e').toInt();
        Court_Distance = Serial4.readStringUntil('f').toInt();
    }

    while(Serial5.available() > 0)
    {
        line_value = Serial5.readStringUntil('\n').toInt();

        
        for(int i = 18; i >= 0; i--)
        {
            if(line_value - pow(2, i) >= 0)
            {
                line_value -= pow(2, i);
                line_data[i] = 1;
            }
            else
            {
                line_data[i] = 0;
            }
                
        }

    }

    HoldSensor = digitalRead(34);
    //Serial.println(HoldSensor);
}

    void print_debug_value_()
    {
        for(int i = 0; i < 19; i++)
        {
            Serial.print(line_data[i]);
            Serial.print(", ");
        }
        Serial.println();
    }

    
int GetDetectedSensorCount(){
  int ret = 0;
  for(int i = 0; i < 16; i++)
  {
    if(line_data[i] == 1)
    {
      ret++;
    }
  }

  return ret;
}

void get_line_deg__(){

  if(GetDetectedSensorCount() == 0)
  {
    //反応したセンサーがない
    Line_Deg = -1;
  }
  else
  {
    if(GetDetectedSensorCount() == 1)
    {
      for(int i = 0; i < 16; i++)
      {
        if(line_data[i] == 1)
        {
          Line_Deg = i * 22.5;

          break;
        }
      }

      Line_Deg = (Line_Deg + int(22.5) + 1) % 360;
    }
    else
    { 
      float _line_vecx = 0;
      float _line_vecy = 0;

      float _line_deg = 0;

      for(int i = 0; i < 16; i++)
      {
        if(line_data[i] == 1)
        {
          _line_vecx += cos(radians(line_sensor_degs[i]));
          _line_vecy += sin(radians(line_sensor_degs[i]));
        }
      }

      _line_deg = degrees(atan2(_line_vecy, _line_vecx));
      _line_deg = _line_deg <= 0 ? _line_deg + 360 : _line_deg;
      Line_Deg = (int)(_line_deg + 11.25) % 360;
    }
  }

  for(int i = 16; i < 19; i++)
  {
    line_side_value[i - 16] = line_data[i];
  }

}



//int GetDetectedSensorCount(){
//}


#endif


