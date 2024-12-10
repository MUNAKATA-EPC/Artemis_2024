#include <Arduino.h>
#include <Servo.h>

#include "attacker.hpp"
#include "defender.hpp"

#include "timer.hpp"
#include "kicker.hpp"
#include "vector.hpp"
#include "motor.hpp"

#include "sensors_variables.hpp"

#include "engelline.hpp"

extern Kicker kicker;
extern Servo dribbler;

Timer kick_time;

void attacker_init()
{

}

void attacker_process(int speed)
{
    pid_gyro();

    kicker.loop();
    kick_time.tick();

    if(is_line_movement())
    {
        if(is_line_halfout())
        {
            motor_move(get_line_deg(), 100);
        }
        else
        {
            motor_move(get_line_deg() + 180, 100);
        }
    }
    else
    {
        if(ball_deg == -1)
        {
            motor_move(0, 0);
        }
        else
        {
            if(ball_deg >= 350 || ball_deg <= 10)
            {
                motor_move(0, 95);

                if(hold_value == 1)
                {
                    kick_time.start();

                    if(kick_time.get_value() >= 150)
                    {
                        kicker.kick();
                    }
                }
                else
                {
                    kick_time.stop();
                    kick_time.reset();                    
                }
            }
            else if((ball_deg >= 330 || ball_deg <= 30) && hold_value)
            {
                motor_move(0, 95);

                if(hold_value == 1)
                {
                    kick_time.start();

                    if(kick_time.get_value() >= 150)
                    {
                        kicker.kick();
                    }
                }
                else
                {
                    kick_time.stop();
                    kick_time.reset();                    
                }
            }
            else
            {
                int motor_speed = speed;

                if(ball_deg < 60 || ball_deg > 300)
                {
                    motor_speed = speed;
                }

                if(ball_distance <= 55)
                {
                    motor_move(ball_deg, motor_speed);
                }
                else
                {
                    if(ball_deg <= 60)
                    {
                        motor_move(ball_deg + 80, motor_speed);
                    }
                    else if(ball_deg <= 160)
                    {
                        motor_move(ball_deg + 55, motor_speed);
                    }
                    else if(ball_deg < 180)
                    {
                        motor_move(ball_deg + 80, motor_speed);
                    }
                    else if(ball_deg < 200)
                    {
                        motor_move(ball_deg - 80, motor_speed);
                    }
                    else if(ball_deg <= 300)
                    {
                        motor_move(ball_deg - 55, motor_speed);
                    }
                    else
                    {
                        motor_move(ball_deg - 80, motor_speed);
                    }
                }
            }
        }
    }

    if(USING_WIRELESS == 1)
    {
        if(ally_mode == 0 || ally_mode == 2)
        {
            run_mode = RUN_MODE::DEFENDER_GYRO;
            defender_mode = 2;

            mode_change_limit.reset();
            mode_change_limit.start();
        }
    }

    Serial.println(kick_time.get_value());
}

bool is_angle_goal(int data)
{
    if(data == -1)
        return false;

    if(data < 15 || data > 345)
    {
        return true;
    }

    return false;
}

Timer Goal_Angle_time;
Timer Delay_Kick;
bool is_bringing_mode;

void attacker_process_cam(int deg, int distance, int e_deg, int e_distance, int speed, int mode)
{
    int cam_deg = deg;
    int cam_distance = distance;

    kicker.loop();
    kick_time.tick();
    Goal_Angle_time.tick();
    
    if(Goal_Angle_time.get_value() >= 3000 && !hold_value)
    {
        Goal_Angle_time.reset();
        Goal_Angle_time.stop();
    }

    if(mode == 1)
    {
        if((ball_deg < 80 || ball_deg > 260) && hold_value == 1 && ((cam_deg <= 30 || cam_deg >= 330) && cam_distance <= 110) && !is_bringing_mode)
        {
            if(cam_deg == -1)
            {
                pid_gyro();
            }
            else
            {
                pid_camera(cam_deg);
            }
        }
        else
        {
            pid_gyro();
        }
    }
    else
    {
        if((ball_deg < 80 || ball_deg > 260) && ball_deg != -1 )
        {
            if(cam_deg == -1)
            {
                pid_gyro();
            }
            else
            {
                pid_camera(cam_deg - 3);
            }
        }
        else
        {
            pid_gyro();
        }
    }

    if(is_line_movement())
    {
        if(is_angle_goal(cam_deg) && hold_value == 1 && (ball_deg <= 30 || ball_deg >= 330))
        {
            kicker.kick();
        }


        if(is_line_halfout())
        {
            motor_move(get_line_deg(), 90);
        }
        else
        {
            motor_move(get_line_deg() + 180, 90);
        }
    }
    else
    {
        if(ball_deg == -1)
        {
            dribbler.writeMicroseconds(1000);

            motor_move(0, 0);
        }
        else
        {
            if((ball_deg >= 350 || ball_deg <= 2) || ((ball_deg >= 330 || ball_deg <= 30) && (hold_value == 1)))
            {
                if(hold_value == 1)
                {
                    Goal_Angle_time.start();

                    if(((cam_deg <= 40 || cam_deg >= 320)) && !is_bringing_mode)
                    {
                        dribbler.writeMicroseconds(1200);

                        if((ball_deg >= 340 || ball_deg <= 20))
                        {
                            motor_move(ball_deg, 90);
                        }
                        else
                        {
                            motor_move(0, 90);
                        }

                        if((mode == 1 ? is_angle_goal(cam_deg) && cam_distance <= 110 : true) && Goal_Angle_time.get_value() >= 150)
                        {
                            Goal_Angle_time.reset();
                            kicker.kick();
                        }
                    }
                    else
                    {
                        is_bringing_mode = true;
                        dribbler.writeMicroseconds(1300);

                        if((cam_deg <= 30 || cam_deg >= 330))
                        {
                            is_bringing_mode = false;
                            Goal_Angle_time.reset();
                        }

                        if(Goal_Angle_time.get_value() <= 100)
                        {
                            motor_move(0, 0);
                        }
                        else
                        {
                            float pow = 1 - (1 - (Goal_Angle_time.get_value() - 200) / 800.0) * (1 - (Goal_Angle_time.get_value() - 200) / 800.0);

                            if(cam_deg <= 180)
                            {
                                motor_move(360 - (cam_deg + 110), Goal_Angle_time.get_value() <= 1000 ? pow * 60 : 75);
                            }
                            else
                            {
                                motor_move(360 - (cam_deg - 110), Goal_Angle_time.get_value() <= 1000 ? pow * 60 : 75);
                            }
                        }
                    }
                }
                else
                {
                    motor_move(0, 90);

                    Goal_Angle_time.reset();
                    kick_time.stop();
                    kick_time.reset();                    
                }
            }
            else
            {
                Goal_Angle_time.reset();

                dribbler.writeMicroseconds(1050);

                int motor_speed = speed;
                
                if(ball_deg < 60 || ball_deg > 300)
                {
                    motor_speed = speed * 0.8;
                }

                if(ball_distance <= 55)
                {
                    motor_move(ball_deg, motor_speed);  
                }
                else
                {
                    if(ball_deg <= 60)
                    {
                        motor_move(ball_deg + 55, motor_speed);
                    }
                    else if(ball_deg <= 160)
                    {
                        motor_move(ball_deg + 55, motor_speed);
                    }
                    else if(ball_deg < 180)
                    {
                        motor_move(ball_deg + 80, motor_speed);
                    }
                    else if(ball_deg < 200)
                    {
                        motor_move(ball_deg - 80, motor_speed);
                    }
                    else if(ball_deg <= 320)
                    {
                        motor_move(ball_deg - 55, motor_speed);
                    }
                    else
                    {
                        motor_move(ball_deg - 55, motor_speed);
                    }
                }
            }
        }
    }
}