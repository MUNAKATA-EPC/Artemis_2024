#include <Arduino.h>

int Gyro_deg_360;
int Gyro_Deg;
float Ball_Deg;
float Ball_Distance;
float Ball_Distance_a;
int Line_Deg;
int Line_Distance;

String str;

int Cam_Court_Deg;
int Cam_Court_Dis;
int Cam_GoalY_Deg;
int Cam_GoalY_Dis;
int Cam_GoalB_Deg;
int Cam_GoalB_Dis;


void serials_init()
{
    Serial2.begin(115200);
    Serial2.setTimeout(10);

    Serial3.begin(19200);
    Serial3.setTimeout(10);

    Serial4.begin(115200);
    Serial4.setTimeout(10);

    Serial5.begin(115200);
    Serial5.setTimeout(30);
}

void serials_receive()
{
    while(Serial2.available() > 0)
    {
        Gyro_Deg = Serial2.readString().toInt();
        Gyro_deg_360 = (((Gyro_Deg * 360 / 1024 - 180 + 360) % 360) -360) * -1;
    }
    
    while(Serial3.available() > 0)
    {
        Ball_Deg = Serial3.readStringUntil('a').toInt();
        Ball_Distance = Serial3.readStringUntil('b').toInt();
        Ball_Distance = 100 - Ball_Distance;
    }
    while(Serial4.available() > 0)
    {
        Cam_GoalY_Deg = Serial4.readStringUntil('a').toInt();
        Cam_GoalY_Dis = Serial4.readStringUntil('b').toInt();
        Cam_GoalB_Deg = Serial4.readStringUntil('c').toInt();
        Cam_GoalB_Dis = Serial4.readStringUntil('d').toInt();
        Cam_Court_Deg = Serial4.readStringUntil('e').toInt();
        Cam_Court_Dis = Serial4.readStringUntil('f').toInt(); 
        Cam_Court_Deg = Cam_Court_Deg * 2;
    }    

    while(Serial5.available() > 0)
    {
        Line_Deg = Serial5.readStringUntil('a').toInt();
        Line_Distance = Serial5.readStringUntil('b').toInt();
    } 
}

  
