#include <Arduino.h>
#include "sensors_variables.hpp"
#include "motor.hpp"

#define GYRO_PID_GAIN_P 0.5
#define GYRO_PID_GAIN_I 0
#define GYRO_PID_GAIN_D 1.5

#define CAM_PID_GAIN_P 1.65
#define CAM_PID_GAIN_I 0
#define CAM_PID_GAIN_D 2.1
int pid_now_deg;
int pid_before_deg;

int pid_deviation;
int pid_before_deviation;

int pid_val;

void pid_gyro()
{
    //PID制御もどき
    int p_val = 0, i_val = 0, d_val = 0;

    pid_now_deg = gyro_data;

    p_val = (gyro_data);

    pid_deviation = p_val;
    d_val = pid_deviation - pid_before_deviation;
    pid_before_deviation = pid_deviation;

    pid_before_deg = gyro_data;

    pid_val = p_val * GYRO_PID_GAIN_P + d_val * GYRO_PID_GAIN_D;

}

void pid_camera(int value, float p_gain = 0, float d_gain = 0)
{
    if(value >= 90)
        value = value - 180;

    //PID制御もどき
    int p_val, i_val, d_val;

    pid_now_deg = value;

    p_val = (value);

    pid_deviation = p_val;
    d_val = pid_deviation - pid_before_deviation;
    pid_before_deviation = pid_deviation;

    pid_before_deg = value;

    float p_true_gain = p_gain == 0 ? CAM_PID_GAIN_P : p_gain;
    float d_true_gain = d_gain == 0 ? CAM_PID_GAIN_D : d_gain;

    pid_val = p_val * p_true_gain + d_val * d_true_gain;
}

void motor_init()
{
    Serial1.begin(115200);
    Serial1.setTimeout(10);
}

void motor_direct_drive(int a, int b, int c)
{
    String power_a = String(min(max(abs(a), 0), 100));
    String power_b = String(min(max(abs(b), 0), 100));
    String power_c = String(min(max(abs(c), 0), 100));

    if(abs(a) <= 9)
    {
        power_a = "00" + power_a;
    }
    else if(abs(a) <= 99)
    {
        power_a = "0" + power_a;
    }

    if(abs(b) <= 9)
    {
        power_b = "00" + power_b;
    }
    else if(abs(b) <= 99)
    {
        power_b = "0" + power_b;
    }

    if(abs(c) <= 9)
    {
        power_c = "00" + power_c;
    }
    else if(abs(c) <= 99)
    {
        power_c = "0" + power_c;
    }

    String rotate_a = a < 0 ? "1" : "0";
    String rotate_b = b < 0 ? "1" : "0";
    String rotate_c = c < 0 ? "1" : "0";

    String send_data = rotate_a + power_a + rotate_b + power_b + rotate_c + power_c;

    Serial1.print(send_data);
}

void motor_move(int deg, int power)
{
    //角度による計算
    float powers[3] = {0, 0, 0};

    powers[0] = sin(radians(deg + 60)) * -power;
    powers[1] = sin(radians(deg - 60)) * -power;
    powers[2] = sin(radians(deg - 180)) * -power;

    float strongest_power = 0;

    for(int i = 0; i < 3; i++)
    {
        if(abs(powers[i]) > strongest_power)
        {
            strongest_power = abs(powers[i]);
        }
    }   

    if(strongest_power != 0)
    {
        float ratio_of_max_power = power / strongest_power;

        for(int i = 0; i < 3; i++)
        {
            powers[i] = (powers[i] * ratio_of_max_power);
        }
    }

    for(int i = 0; i < 3; i++)
    {
        powers[i] -= pid_val;

        if(powers[i] > 100)
        {
            powers[i] = 100;
        }
        else if(powers[i] < -100)
        {
            powers[i] = -100;
        }
    }

    motor_direct_drive((int)powers[0], (int)powers[1], (int)powers[2]);
}