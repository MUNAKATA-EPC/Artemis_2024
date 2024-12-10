#include <Arduino.h>

#include "bigfield_attacker.hpp"

#include "timer.hpp"
#include "kicker.hpp"
#include "vector.hpp"
#include "motor.hpp"

#include "sensors_variables.hpp"

#include "engelline.hpp"

extern Kicker kicker;

Timer kick_time;

void attacker_init()
{

}

void bigfield_attacker_process(int speed)
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
                motor_move(0, 70);

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
                motor_move(0, 70);

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
                    motor_speed = speed * 0.6;
                }

                if(ball_distance <= 55)
                {
                    motor_move(ball_deg, motor_speed);
                }
                else
                {
                    if(ball_deg <= 60)
                    {
                        motor_move(ball_deg + 50, motor_speed);
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
                        motor_move(ball_deg - 45, motor_speed);
                    }
                }
            }
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

void bigfield_attacker_process_cam(int deg, int distance, int speed)
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

    if(ball_deg < 80 || ball_deg > 280)
    {
        if(cam_deg == -1)
        {
            pid_gyro();
        }
        else
        {
            pid_camera(cam_deg - 5);
        }
    }
    else
    {
        pid_gyro();
    }

    if(is_line_movement())
    {
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
            motor_move(0, 0);
        }
        else
        {
            if(ball_deg >= 345 || ball_deg <= 15)
            {
                motor_move(0, 70);

                if(hold_value == 1)
                {
                    Goal_Angle_time.start();
                    
                    if(Goal_Angle_time.get_value() > 300)
                    {
                        Goal_Angle_time.reset();
                        kicker.kick();
                    }
                }
                else
                {
                    kick_time.stop();
                    kick_time.reset();                    
                }
            }
            else if((ball_deg >= 330 || ball_deg <= 30))
            {
                if(hold_value)
                {
                    motor_move(0, 70);

                    if(hold_value == 1)
                    {
                        Goal_Angle_time.start();

                        if(Goal_Angle_time.get_value() > 300)
                        {
                            Goal_Angle_time.reset();
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
                    if(ball_deg >= 330)
                    {
                        motor_move(360 - (360 - ball_deg) * 2.0, 70);
                    }
                    else
                    {
                        motor_move(ball_deg * 2.4, 70);
                    }
                }
            }
            else
            {
                int motor_speed = speed;
                
                if(ball_deg < 60 || ball_deg > 300)
                {
                    motor_speed = speed * 0.5;
                }

                
                if(ball_distance <= 55)
                {
                    motor_move(ball_deg, motor_speed);  
                }
                else
                {
                    if(ball_deg <= 60)
                    {
                        motor_move(ball_deg + 50, motor_speed);
                    }
                    else if(ball_deg <= 160)
                    {
                        motor_move(ball_deg + 55, motor_speed * 0.9);
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
                        motor_move(ball_deg - 55, motor_speed * 0.9);
                    }
                    else
                    {
                        motor_move(ball_deg - 45, motor_speed);
                    }
                }
            }
        }
    }
}