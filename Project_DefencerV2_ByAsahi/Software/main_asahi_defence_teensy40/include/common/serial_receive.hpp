#ifndef _SERIAL_RECEIVE_H
#define _SERIAL_RECEIVE_H

#include <Arduino.h>

int Gyro_Deg;

float Ball_Deg = -1;
float Ball_Distance = -1;

int Line_Deg = -1;
int Line_Distance = -1;

int Yellow_Goal_Deg;
int Yellow_Goal_Distance;
int Blue_Goal_Deg;
int Blue_Goal_Distance;
int Court_Deg;
int Court_Distance;

int HoldSensor;

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
    Serial5.setTimeout(50);
}

void serials_receive()
{
    while(Serial2.available() > 0)
    {
        Gyro_Deg = Serial2.readString().toInt();
    }

    while(Serial3.available() > 0)
    {
        Ball_Deg = Serial3.readStringUntil('a').toInt();
        Ball_Distance = Serial3.readStringUntil('b').toInt();
    }

    while(Serial4.available() > 0)
    {
        Yellow_Goal_Deg = Serial4.readStringUntil('a').toInt();
        Yellow_Goal_Distance = Serial4.readStringUntil('b').toInt();
        Blue_Goal_Deg = Serial4.readStringUntil('c').toInt();
        Blue_Goal_Distance = Serial4.readStringUntil('d').toInt();
        Blue_Goal_Deg = Serial4.readStringUntil('e').toInt();
        Blue_Goal_Distance = Serial4.readStringUntil('f').toInt();
    }

    while(Serial5.available() > 0)
    {
        Line_Deg = Serial5.readStringUntil('a').toInt();
        Line_Distance = Serial5.readStringUntil('b').toInt();
    }

    HoldSensor = digitalRead(34);

    Serial.println(HoldSensor);
}

#endif
