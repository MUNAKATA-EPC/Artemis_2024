#ifndef _SENSORS_VARIABLES_HPP
#define _SENSORS_VARIABLES_HPP

#include <Arduino.h>

//ボール情報
extern int ball_deg;
extern int ball_distance;

//ボール中心座標(左、右)
extern int ball_centers[2];

//ゴール情報
extern int yellow_goal_deg;
extern int yellow_goal_distance;
extern int blue_goal_deg;
extern int blue_goal_distance;

//ジャイロ情報
extern float gyro_deg_m5stickc;
extern int gyro_deg_l3gd20h;
extern float gyro_calibration_rtrim_data;

extern float gyro_data;

//ライン情報
extern String line_data;
extern int line_value;

//ホールドセンサ情報
extern int hold_value;

//お遊び
extern int orange_ball_deg;
extern int orange_ball_distance;

#endif