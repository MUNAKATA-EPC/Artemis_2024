#ifndef _MOTOR_H
#define _MOTOR_H

#include <Arduino.h>
#include "DSR1202.h"

DSR1202 md = DSR1202(0);

float P_GAIN = 0.10;
float D_GAIN = 2.0;

float P_GAIN_CAM = 0.5;
float D_GAIN_CAM = 2;

int deviation;
int old_deviation;

int pid_val = 0;

class PID_Motion{
public:
    int pid_gyro()
    {
        //ジャイロ角度を用いたPIDでの角度制御


        int p_val = (Gyro_Deg - 512) * P_GAIN;   
        int d_val = 0;

        deviation = p_val;
        d_val = (deviation - old_deviation);
        old_deviation = deviation;  

        d_val = d_val * D_GAIN;

        pid_val = p_val + d_val;

        return 0;
    }

    int pid_camera(int deg)
    {
        //カメラ角度を用いたPIDでの角度制御

        int p_val = (deg >= 90 ? deg - 180 : deg) * P_GAIN_CAM;   
        int d_val = 0;

        deviation = p_val;
        d_val = (deviation - old_deviation);
        old_deviation = deviation;  

        d_val = d_val * D_GAIN_CAM;

        pid_val = p_val + d_val;

        return 0;
    }
};

PID_Motion pid_motion;

void motor_init(){
    md.Init();
    md.move(0, 0, 0, 0);
}

void motor_power(float* powers){
    md.move((int)(-powers[0]), (int)(-powers[1]), (int)(-powers[2]), (int)(-powers[3]));            
}

void motor_power(float a, float b, float c, float d){
    md.move(-a, -b, -c, -d);
}

void move(int deg, int power){
    float motor_powers[4];

    //モーターの角度による出力値計算
    motor_powers[0] = -sin(radians(deg - 50)) * power;
    motor_powers[1] = sin(radians(deg + 50)) * power;
    motor_powers[2] = -sin(radians(deg - 130)) * power;
    motor_powers[3] = sin(radians(deg + 130)) * power;

    float max_motor_power = 0;

    for(int i = 0; i < 4; i++)
    {
        if(max_motor_power < abs(motor_powers[i]))
        {
            max_motor_power = abs(motor_powers[i]);
        }
    }

    float motor_power_mul = power / max_motor_power;

    
    for(int i = 0; i < 4; i++)
    {
        motor_powers[i] = motor_powers[i] * motor_power_mul;
    }
    
    
    if(power == 0)
    {
        for(int i = 0; i < 4; i++)
        {
            motor_powers[i] = 0;
        }
    }

    motor_powers[0] = motor_powers[0] - pid_val;
    motor_powers[1] = motor_powers[1] + pid_val;
    motor_powers[2] = motor_powers[2] - pid_val;
    motor_powers[3] = motor_powers[3] + pid_val;

    for(int i = 0; i < 4; i++)
    {
        motor_powers[i] = min(max(motor_powers[i], -100), 100);
    }

    //モーター出力

    motor_power(motor_powers);
}

void motor_break()
{
    if(Gyro_Deg >= 500 && Gyro_Deg <= 524)
    {
        md.motor_break();
    }
    else
    {
        move(0, 0);
    }
}

#endif