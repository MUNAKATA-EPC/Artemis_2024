#ifndef _MOTOR_HPP
#define _MOTOR_HPP

#include <Arduino.h>
#include "DSR1202.h"
#include "serial_receive.hpp"

DSR1202 md = DSR1202(0);

//origin = 0.13
float P_GAIN = 0.6;
float I_GAIN = 0.1;
float D_GAIN = 4.2;

float P_GAIN_CAM = 0.55;
float D_GAIN_CAM = 2.2;

int deviation;
int old_deviation;

int pid_val = 0;

int now_time;
int old_time;

class PID_Motion{
private:
    int gyro_pid_goal = 180;
public:
    float p_val_g;
    float i_val_g;
    float d_val_g;
    
    int pid_gyro()
    {
        //ジャイロ角度を用いたPIDでの角度制御
        now_time = millis();
        
        //float dt = (now_time - old_time) / 1000;

        p_val_g = (360 -  Gyro_Deg - gyro_pid_goal);
                
        deviation = p_val_g;
        d_val_g = deviation - old_deviation;
        old_deviation = deviation;  

        pid_val = (int)(p_val_g * P_GAIN + d_val_g * D_GAIN);

        old_time = now_time;

        return 0;
    }

    void set_pid_goal(int data)
    {
        gyro_pid_goal = data;
    }

    void set_pid_gain(int p)
    {
        P_GAIN = p;
    }

    int pid_camera(int deg)
    {
        //カメラ角度を用いたPIDでの角度制御

        int p_val = (deg >= 90 ? deg - 180 : deg) * P_GAIN_CAM ;   
        int d_val = 0;

        deviation = p_val;
        d_val = (deviation - old_deviation);
        old_deviation = deviation;  

        pid_val = p_val + d_val * D_GAIN_CAM;

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
    motor_powers[0] = -sin(radians(deg - 60)) * power;
    motor_powers[1] = sin(radians(deg + 60)) * power;
    motor_powers[2] = -sin(radians(deg - 135)) * power;
    motor_powers[3] = sin(radians(deg + 135)) * power;

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

    int pid_fixed_val = pid_val >= (100 - max_motor_power) ? (100 - max_motor_power) : pid_val;

    motor_powers[0] = motor_powers[0] - pid_fixed_val;
    motor_powers[1] = motor_powers[1] + pid_fixed_val;
    motor_powers[2] = motor_powers[2] - pid_fixed_val;
    motor_powers[3] = motor_powers[3] + pid_fixed_val;

    for(int i = 0; i < 4; i++)
    {
        motor_powers[i] = min(max(motor_powers[i], -100), 100);
    }

    //モーター出力

    motor_power(motor_powers);
}

void motor_break()
{
    md.motor_break();
}

#endif