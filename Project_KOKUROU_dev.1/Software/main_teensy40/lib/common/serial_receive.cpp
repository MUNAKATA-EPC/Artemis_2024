#include <Arduino.h>
#include "serial_receive.hpp"
#include "sensors_variables.hpp"

int ball_deg = -1;
int ball_distance = -1;

int ball_centers[2] = {338, 20};

int yellow_goal_deg = -1;
int yellow_goal_distance = -1;
int blue_goal_deg = -1;
int blue_goal_distance = -1;

float gyro_deg_m5stickc = -1;
int gyro_deg_l3gd20h = -1;
float gyro_calibration_rtrim_data = -1;

float gyro_data = -1;

String line_data = "";
int line_value = 0;

int hold_value = -1;

int cal_temp_data;

int orange_ball_deg = -1;
int orange_ball_distance = -1;

int serial_count;

/// @brief 各種シリアルを初期化します。
void serials_init()
{
    pinMode(27, INPUT);

    Serial2.begin(19200);
    Serial2.setTimeout(40);
        
    Serial3.begin(115200);
    Serial3.setTimeout(10);

    Serial5.begin(115200);
    Serial5.setTimeout(10);

    pinMode(30, INPUT_PULLDOWN);
    pinMode(16, INPUT_PULLDOWN);
}

/// @brief 各種シリアルからの値を取得し、変換します。
void serials_read()
{
    hold_value = digitalRead(30);
    line_value = analogRead(16) >= 512 ? 1023 : 0;
    gyro_calibration_rtrim_data = (analogRead(27) - 512) / 250.0;

    serial_count = (serial_count + 1) % 4;

    
    while(Serial2.available() > 0)
    {
        ball_deg = Serial2.readStringUntil('a').toInt();
        ball_distance = Serial2.readStringUntil('b').toInt();
    }
    
    while(Serial3.available() > 0)
    {
        gyro_data = Serial3.readString().toFloat();
    }

    while(Serial5.available() > 0)
    {
        yellow_goal_deg = Serial5.readStringUntil('a').toInt();
        yellow_goal_distance = Serial5.readStringUntil('b').toInt();
        blue_goal_deg = Serial5.readStringUntil('c').toInt();
        blue_goal_distance = Serial5.readStringUntil('d').toInt();
        orange_ball_deg = Serial5.readStringUntil('e').toInt();
        orange_ball_distance = Serial5.readStringUntil('f').toInt();
    }

    Serial.println(line_value);
}