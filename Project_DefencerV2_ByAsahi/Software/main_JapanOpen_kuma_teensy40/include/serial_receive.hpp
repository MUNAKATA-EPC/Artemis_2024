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



int Yellow_Goal_Deg;
int Yellow_Goal_Distance;
int Blue_Goal_Deg;
int Blue_Goal_Distance;
int Court_Deg;
int Court_Distance;
int HoldSensor;

int raw_deg;
int reset_deg;

void serials_init()
{
    Serial.begin(9600);

    Serial2.begin(115200);
    Serial2.setTimeout(10);

    Serial3.begin(19200);
    Serial3.setTimeout(10);

    Serial4.begin(115200);
    Serial4.setTimeout(10);

    Serial5.begin(115200);

    pinMode(10, INPUT_PULLDOWN);
    
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
    while(Serial3.available() > 0)
    {
        Ball_Deg = Serial3.readStringUntil('a').toInt();
        Ball_Distance_ = Serial3.readStringUntil('b').toInt();

        if(Ball_Distance_ != -1)
        {
            Ball_Distance = 110 - Ball_Distance_;
        }
    }

    while(Serial4.available() > 0)
    {
        Yellow_Goal_Deg = Serial4.readStringUntil('a').toInt();
        Yellow_Goal_Distance = Serial4.readStringUntil('b').toInt();
        Blue_Goal_Deg = Serial4.readStringUntil('c').toInt();
        Blue_Goal_Distance = Serial4.readStringUntil('d').toInt();
        Court_Deg = Serial4.readStringUntil('e').toInt();
        Court_Distance = Serial4.readStringUntil('f').toInt();
    }

    if(Serial5.available() > 0)
    {
        Line_Deg = Serial5.readStringUntil('a').toInt();
        Line_right = Serial5.readStringUntil('b').toInt();
        Line_left = Serial5.readStringUntil('c').toInt();
        Line_back = Serial5.readStringUntil('d').toInt();
    }


    Serial.println(Line_Deg);
    HoldSensor = digitalRead(34);
}

#endif
