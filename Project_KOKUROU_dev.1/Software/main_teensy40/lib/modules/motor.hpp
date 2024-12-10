#ifndef _MOTOR_HPP
#define _MOTOR_HPP

extern void pid_gyro();
extern void pid_camera(int value, float p_gain = 0, float d_gain = 0);
extern void motor_init();
extern void motor_direct_drive(int a, int b, int c);
extern void motor_move(int deg, int power);

#endif