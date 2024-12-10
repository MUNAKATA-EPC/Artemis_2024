#ifndef _MAIN_ATTACK_HPP
#define _MAIN_ATTACK_HPP

#include "serial_receive.hpp"
#include "motor.hpp"
#include "kicker.hpp"
#include "engelline.hpp"
#include "timer.hpp"


int attack_goal ;
class Attacker{
private:
    int before_line;
    bool line_move;
    Timer line;
    Timer line_half;
    int camera_data[4];
    
    #define LINE_SPEED 70
public:

    Timer kick;
void set_camera(int type)
{
    if(type == 0)
    {
        camera_data[0] = Yellow_Goal_Deg;
        camera_data[1] = Yellow_Goal_Distance;
        camera_data[2] = Blue_Goal_Deg;
        camera_data[3] = Blue_Goal_Distance;
    }
    else
    {
        camera_data[0] = Blue_Goal_Deg;
        camera_data[1] = Blue_Goal_Distance;
        camera_data[2] = Yellow_Goal_Deg;
        camera_data[3] = Yellow_Goal_Distance;
    }
}

void main()
{
    int move_deg = 0;
    int move_speed = 0;
    kicker.tick();
    line.tick();
    line_half.tick();

    
    set_camera(attack_goal);
    
    if(camera_data[0] != 255 && (Ball_Deg < 50 || Ball_Deg > 310))
    {
        if((Gyro_Deg > 45 && Gyro_Deg < 135) || (Gyro_Deg > 225 && Gyro_Deg < 315))
        {
            pid_motion.pid_gyro();
        }
        else
        {
            pid_motion.pid_camera(camera_data[0]);
        }
    }
    else
    {
        pid_motion.pid_gyro();
       // pid_motion.pid_camera(camera_data[0]);
    }

    if(is_engelline_detected())
    {
            move_deg = Court_Deg * 2;
            move_speed = LINE_SPEED;
    }
    else if(digitalRead(11) == 1 || digitalRead(12) == 1)
    {
            move_deg = Court_Deg * 2;
            move_speed = LINE_SPEED;

    }
    else if(line_side_value[0] == 1)
    {
            move_deg = 270;
            move_speed = LINE_SPEED;

    }
    else if(line_side_value[1] == 1)
    {
            move_deg = 90;
            move_speed = LINE_SPEED;

    }
    else if(line_side_value[2] == 1)
    {
    
            move_deg = Court_Deg * 2;
            move_speed = LINE_SPEED;
        
    }           
    else
    {
        if(HoldSensor == 1)
        {
        
            move_deg = 0;
            move_speed = 65;
            kicker.kick();
                

        }

    
        if(Ball_Deg == -1)
        {
            move_deg = 0;
            move_speed = 0;
        }
        else
        {
            if(Ball_Deg >= 345 || Ball_Deg <= 15)
            {
                
                    if(Ball_Deg >= 348 || Ball_Deg <= 12)
                    {
                        move_deg = 0;
                        move_speed = 55;
                    }
                    else
                    {
                        move_deg = Ball_Deg;
                        move_speed = 50;
                    }
            }
            else
            {
                int motor_speed = 55;

                if(Ball_Deg <= 65 || Ball_Deg >= 315)
                {
                    motor_speed = 50;
                }

                
                if(Ball_Distance >= 60)
                {   
                    motor_speed =60;
                }

                if(Ball_Distance >= 50)
                {
                    move_deg = Ball_Deg;
                    move_speed =  motor_speed;

                }
                else
                {
                    if(Ball_Deg <= 45)
                    {
                            move_deg = Ball_Deg + 45;
                            move_speed =  motor_speed;
        
                    }
                    else if(Ball_Deg <= 160)
                    {

                        move_deg = Ball_Deg + 50;
                        move_speed =  motor_speed;
                    }
                    else if(Ball_Deg <= 180)
                    {
                        move_deg = Ball_Deg + 85;
                        move_speed =  motor_speed;
                    }
                    else if(Ball_Deg <= 200)
                    {
                        move_deg = Ball_Deg - 85;
                        move_speed =  motor_speed;
                    }
                    else if(Ball_Deg <= 315)
                    {
                        move_deg = Ball_Deg - 50;
                        move_speed = motor_speed;
                    }
                    else
                    {
                            move_deg = Ball_Deg - 45;
                            move_speed =  motor_speed;
     
                    }
                }
            }
        }
    }


    move(move_deg, move_speed);
}
};


Attacker attack;

#endif
