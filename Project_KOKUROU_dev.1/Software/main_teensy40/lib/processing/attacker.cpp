#include <Arduino.h>
#include <Servo.h>
#include "attacker.hpp"

#include "timer.hpp"
#include "kicker.hpp"
#include "vector.hpp"
#include "sensors_variables.hpp"
#include "motor.hpp"
#include "line.hpp"

//extern群
extern Kicker kicker;
extern Servo dribbler;

//変数定義群
Timer hold_time;
Timer ball_bringing_time;
Timer line_time;
Timer avoid_enemy_time;
Timer limit_of_sidemove;
int time_of_starting_avoid;


bool is_ball_center()
{
    if((ball_deg <= ball_centers[1] || ball_deg >= ball_centers[0]) && ball_distance >= 130) 
    {
        return true;
    }
    else if((ball_distance <= 10 || ball_deg >= 350) && ball_distance < 130)
    {
        return true;
    }

    return false;
}

void attacker_init()
{
    hold_time.stop();
    hold_time.reset();
    line_time.reset();
    line_time.stop();
    line_time.set_value(1000);
}

void attacker_gyro_loop()
{
    int speed = 65;

    pid_gyro();

    if(is_line_process())
    {        
        motor_move(orange_ball_deg * 2, 90);
    }
    else
    {
        if(ball_deg == -1)
        {
            motor_move(0, 0);
        }
        else
        {
            if(is_ball_center())
            {
                if(hold_value == 1)
                {
                    kicker.kick();
                }
                
                motor_move(ball_deg, speed);
            }
            else
            {
                if((ball_deg <= 40 && ball_deg >= ball_centers[1]) || (ball_deg >= 320 && ball_deg <= ball_centers[0]))
                {
                    if(ball_distance >= 140)
                    {
                        speed = speed * 0.8;
                    }
                }

                if(ball_deg < 90)
                {
                    Vector VecToBall = Vector(radians(ball_deg), max(20 - (ball_distance - 120), 0));  //ボール方向の要素
                    Vector VecToTanOfBall = Vector(radians(ball_deg + 90), 14);
                    VecToTanOfBall.add(VecToBall);
                    motor_move(VecToTanOfBall.get_deg(), ball_distance >= 130 ? speed : speed + 5);
                }
                else if(ball_deg < 180)
                {
                    Vector VecToBall = Vector(radians(ball_deg), max(25 - (ball_distance - 110), 0));  //ボール方向の要素
                    Vector VecToTanOfBall = Vector(radians(ball_deg + 90), 14);
                    VecToTanOfBall.add(VecToBall);
                    motor_move(VecToTanOfBall.get_deg(), ball_distance >= 100 ? speed + 10 : speed + 20);
                }
                else if(ball_deg < 270)
                {
                    Vector VecToBall = Vector(radians(ball_deg), max(25 - (ball_distance - 120), 0));  //ボール方向の要素
                    Vector VecToTanOfBall = Vector(radians(ball_deg - 90), 14);
                    VecToTanOfBall.add(VecToBall);
                    motor_move(VecToTanOfBall.get_deg(), ball_distance >= 100 ? speed + 10 : speed + 20);
                }
                else 
                {
                    Vector VecToBall = Vector(radians(ball_deg), max(30 - (ball_distance - 120), 0));  //ボール方向の要素
                    Vector VecToTanOfBall = Vector(radians(ball_deg - 90), 14);
                    VecToTanOfBall.add(VecToBall);
                    motor_move(VecToTanOfBall.get_deg(), ball_distance >= 130 ? speed : speed + 5);
                }
            }
        }
    }
}

#define KICK_START_TIME 300

//[0] : ゴール角度 
//[1] : ゴール距離
int Cam_Data[4];

bool was_avoid_enemy;

bool is_front_of_camera()
{
    return (Cam_Data[0] <= 10 || Cam_Data[0] >= 170) && Cam_Data[0] != 255;
}
//TODO:回り込みの修正、もう少し中央に寄るように。
void attacker_cam_loop(int type)
{
    int speed = 90;

    kicker.loop();

    hold_time.tick();
    line_time.tick();
    avoid_enemy_time.tick();
    ball_bringing_time.tick();
    limit_of_sidemove.tick();
    
    if(type == 0)
    {
        Cam_Data[0] = yellow_goal_deg;
        Cam_Data[1] = yellow_goal_distance;
        Cam_Data[2] = blue_goal_deg;
        Cam_Data[3] = blue_goal_distance;
    }
    else
    {
        Cam_Data[0] = blue_goal_deg;
        Cam_Data[1] = blue_goal_distance;
        Cam_Data[2] = yellow_goal_deg;
        Cam_Data[3] = yellow_goal_distance;
    }

    if(hold_time.get_value() >= 50 && hold_time.get_value() <= KICK_START_TIME + 600 && Cam_Data[0] != 255)
    {
        pid_camera(Cam_Data[0]);
    }
    else
    {
        pid_gyro();
    }

    //敵避け処理
    if(hold_value == 1 && !ball_bringing_time.is_ticking())
    {
        avoid_enemy_time.start();
    }
    else
    {
        avoid_enemy_time.reset();
        avoid_enemy_time.stop();
        time_of_starting_avoid = -1;
    }
    
    if(avoid_enemy_time.get_value() >= 6000)
    {
        avoid_enemy_time.reset();
        avoid_enemy_time.stop();
        time_of_starting_avoid = -1;
    }

    if(avoid_enemy_time.get_value() >= 3000 && time_of_starting_avoid == -1)
    {
        time_of_starting_avoid = 1;
    }

    //ゴールサイドでの速度減速
    if(Cam_Data[0] >= 30 && Cam_Data[0] <= 150)
    {
        speed = 65;
    }

    //ライン処理
    if(is_line_process())
    {
        if(line_time.get_value() > 100)
        {
            line_time.reset();
            line_time.start();  
        }
    }

    if(line_time.get_value() <= 200)
    {
        if(is_line_process())
        {
            motor_move(orange_ball_deg * 2, 100);
        }
        else
        {
            motor_move(orange_ball_deg * 2, 100);
        }

        if(hold_value == 1 && (Cam_Data[0] <= 20 || Cam_Data[0] >= 160))
        {
            kicker.kick();
        }
    }
    else
    {
        if(is_line_process())
        {
            line_time.set_value(150);
            
            if(Cam_Data[0] >= 30 && Cam_Data[0] <= 150)
            {
                if(
                    hold_value == 1)
                {
                    motor_move(orange_ball_deg * 2, 30); 
                }
                else
                {
                    motor_move(orange_ball_deg * 2, 100);
                }
            }
            else
            {
                motor_move(orange_ball_deg * 2, 100);

                if(hold_value == 1)
                {
                    kicker.kick();
                }
            }
        }
        else
        {
            if(ball_deg == -1)
            {
                motor_move(180, 40);
            }
            else
            {
                if((hold_value == 1 && (ball_deg <= 30 || ball_deg >= 330)))      //ホールドセンサがボールをつかんでいると判断したら
                {                
                    if(time_of_starting_avoid == 1)
                    {
                        dribbler.writeMicroseconds(1150);

                        if(Cam_Data[0] <= 90)
                        {
                            motor_move(135, 80);
                        }
                        else
                        {
                            motor_move(225, 80);
                        }
                    }
                    else
                    {
                        if(Cam_Data[0] != 255)
                        {
                            if((Cam_Data[0] >= 20 && Cam_Data[0] <= 160) && !hold_time.is_ticking())
                            {
                                int bring_speed = 85;

                                if(!(Cam_Data[0] >= 30 && Cam_Data[0] <= 150))
                                {
                                    bring_speed = 65;
                                }

                                ball_bringing_time.start();

                                if(ball_bringing_time.get_value() <= 200)
                                {
                                    dribbler.writeMicroseconds(1300);
                                    motor_move(0, 0);
                                }
                                else
                                {
                                    dribbler.writeMicroseconds(1250);

                                    if(Cam_Data[0] < 90)
                                    {
                                        motor_move(min(Cam_Data[0] * 2 + 100, 170), bring_speed);
                                    }
                                    else
                                    {
                                        motor_move(max(Cam_Data[0] * 2 - 100, 190), bring_speed);
                                    }
                                }
                            }
                            else
                            {
                                hold_time.start(); 
                                
                                if(hold_time.get_value() <= KICK_START_TIME)
                                {
                                    was_avoid_enemy = false;
                                    dribbler.writeMicroseconds(1100);

                                    if(Cam_Data[0] != 255)
                                    {
                                        if(Cam_Data[1] < 90)
                                        {
                                            motor_move(0, 65);
                                        }
                                        else
                                        {
                                            if(Cam_Data[0] < 90)
                                            {
                                                motor_move(Cam_Data[0], 80);
                                            }
                                            else 
                                            {
                                                motor_move(360 - (((360 - Cam_Data[0] * 2)) / 2), 80);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        motor_move(0, 90);
                                    }
                                }
                                else
                                {
                                    dribbler.writeMicroseconds(1000);

                                    if(ball_bringing_time.is_ticking())
                                    {
                                        ball_bringing_time.reset();
                                        ball_bringing_time.stop();
                                    }
                                    
                                    if(hold_value == 0)
                                    {
                                        dribbler.writeMicroseconds(1000);
                                        hold_time.stop();
                                        hold_time.reset();
                                        motor_move(0, 70);
                                    }
                                    else
                                    {
                                        if(Cam_Data[1] <= 90)
                                        {
                                            motor_move(0, 65);
                                        }
                                        else
                                        {
                                            if(is_front_of_camera())
                                            {
                                                kicker.kick();
                                                motor_move(0, 70);
                                            }
                                        }
                                        /*
                                        else
                                        {
                                            was_avoid_enemy = true;
                                            if(Cam_Data[0] <= 90)
                                            {
                                                motor_move(90, speed + 10);
                                            }
                                            else
                                            {
                                                motor_move(270, speed + 10);
                                            }

                                            dribbler.writeMicroseconds(1200);
                                        }*/
                                    }
                                }
                            }
                        }
                        else
                        {
                            dribbler.writeMicroseconds(1100);
                            hold_time.start();
                            if(hold_time.get_value() >= 500)
                            {
                                kicker.kick();
                            }
                        }
                    }
                }
                else
                {
                    if(ball_distance <= 95)
                    {
                        motor_move(ball_deg, 75);
                    }
                    else
                    {
                        ball_bringing_time.reset();
                        ball_bringing_time.stop();
                        hold_time.stop();
                        hold_time.reset();

                        if(ball_deg <= 18 || ball_deg >= 342)
                        {
                            dribbler.writeMicroseconds(1060);

                            motor_move(0, speed);
                        }
                        else if((ball_deg <= 30 || ball_deg >= 330))
                        {
                            dribbler.writeMicroseconds(1060);

                            if(ball_deg <= 180)
                            {
                                motor_move(ball_deg + 15, speed);
                            }
                            else
                            {
                                motor_move(ball_deg - 15, speed);
                            }
                        }
                        else
                        {
                            int move_deg = 0;
                            int move_speed = 0;

                            dribbler.writeMicroseconds(1000);

                            if((ball_deg <= 50) || (ball_deg >= 310))
                            {
                                if(ball_distance >= 100)
                                {
                                    speed = speed * 0.95;
                                }
                            }

                            if(line_time.get_value() <= 600)
                            {
                                speed = speed * 0.8;
                            }

                            if(ball_deg < 90)
                            {
                                Vector VecToBall = Vector(radians(ball_deg), max(30 - (ball_distance - 110), 0));  //ボール方向の要素
                                Vector VecToTanOfBall = Vector(radians(ball_deg + 90), 10);
                                VecToTanOfBall.add(VecToBall);
                                move_deg = VecToTanOfBall.get_deg();
                                move_speed = ball_distance >= 130 ? speed : speed + 5;
                            }
                            else if(ball_deg < 180)
                            {
                                Vector VecToBall = Vector(radians(ball_deg), max(30 - (ball_distance - 100), 0));  //ボール方向の要素
                                Vector VecToTanOfBall = Vector(radians(ball_deg + 90), 17);
                                VecToTanOfBall.add(VecToBall);
                                move_deg = VecToTanOfBall.get_deg();
                                move_speed = ball_distance >= 100 ? speed : speed + 5;
                            }
                            else if(ball_deg < 270)
                            {
                                Vector VecToBall = Vector(radians(ball_deg), max(50 - (ball_distance - 100), 0));  //ボール方向の要素
                                Vector VecToTanOfBall = Vector(radians(ball_deg - 90), 15);
                                VecToTanOfBall.add(VecToBall);
                                move_deg = VecToTanOfBall.get_deg();
                                move_speed = ball_distance >= 100 ? speed : speed + 5;
                            }
                            else 
                            {
                                Vector VecToBall = Vector(radians(ball_deg), max(40 - (ball_distance - 110), 0));  //ボール方向の要素
                                Vector VecToTanOfBall = Vector(radians(ball_deg - 90), 10);
                                VecToTanOfBall.add(VecToBall);
                                move_deg = VecToTanOfBall.get_deg();
                                move_speed = ball_distance >= 130 ? speed : speed + 5;
                            }

                            if(abs(sin(radians(move_deg))) >= abs(sin(radians(45))))
                            {
                                limit_of_sidemove.start();

                                if(limit_of_sidemove.get_value() >= 250)
                                {
                                    if(limit_of_sidemove.get_value() >= 800)
                                    {
                                        limit_of_sidemove.reset();
                                    }
                                    else
                                    {
                                        move_speed = move_speed * 0.95;
                                    }
                                }
                            }
                            else
                            {
                                limit_of_sidemove.reset();
                            }

                            if(ball_distance >= 110)
                            {
                                move_speed = move_speed * 0.8;
                            }

                            motor_move(move_deg, move_speed);
                        }
                    }
                }
                    
            }  
        }
    }
}