#include <Arduino.h>
#include "defender.hpp"

#include "timer.hpp"
#include "kicker.hpp"
#include "vector.hpp"
#include "motor.hpp"

#include "sensors_variables.hpp"

#include "engelline.hpp"
#include "attacker.hpp"

int defender_mode = 0;
int line_trace_mode = 0;

int def_previous_line_deg = -1;

extern Kicker kicker;

Timer attack_change;
Timer attack_change_wait;

void defender_init()
{

}

void defender_process(bool cam_mode, int cam_deg_value, int cam_dis_value, int at_cam_deg, int at_cam_dis)
{
    //まずはライン上の2点を取得する

    kicker.loop();
    attack_change.tick();
    attack_change_wait.tick();

    pid_gyro();

    int line_left_deg = -1;
    int line_right_deg = -1;

    //ボール所持時はアタッカーに瞬時に切り替え
    if(hold_value == 1)
    {
        kicker.kick();
    }

    //ライン上待機 & ボール保守モード
    if(defender_mode == 0)
    {
        if(is_line_detected())
        {
            if(ball_deg == -1)
            {
                motor_move(0, 0);

                return;
            }

            for(int i = 0; i < 10; i++)
            {
                if(line_data[i] == 1)
                {
                    line_right_deg = i * 22.5;
                    break;
                }
            }

            for(int i = 15; i >= 7; i--)
            {
                if(line_data[i] == 1)
                {
                    line_left_deg = i * 22.5;
                    break;
                }
            }

            int _line_central_deg = degrees(atan2(sin(radians(line_left_deg)) + sin(radians(line_right_deg)), cos(radians(line_left_deg)) + cos(radians(line_right_deg))));
            _line_central_deg = _line_central_deg < 0 ? _line_central_deg + 360 : _line_central_deg;
            int _sub_line_central_deg = (_line_central_deg + 180) % 360;

            int line_central_deg[4] = {0, 0, 0, 0};

            if(_sub_line_central_deg < _line_central_deg)
            {
                line_central_deg[0] = _sub_line_central_deg;
                line_central_deg[1] = _line_central_deg;
            }
            else
            {
                line_central_deg[0] = _line_central_deg;
                line_central_deg[1] = _sub_line_central_deg;
            }

            line_central_deg[2] = (line_central_deg[0] + line_central_deg[1]) / 2; 

            if(line_central_deg[2] > (line_central_deg[0] + line_central_deg[1] + 180) % 360)
            {
                line_central_deg[3] = line_central_deg[2];
                line_central_deg[2] = (line_central_deg[0] + line_central_deg[1] + 180) % 360;
            }
            else
            {
                line_central_deg[3] = (line_central_deg[2] + 180) % 360;
            }

            if(line_data[18] == 1)
            {
                line_right_deg = 270;
            }

            if(line_data[16] == 1)
            {
                line_left_deg = 90;
            }



            if (    (((line_left_deg < 30 || line_left_deg > 330) && (line_right_deg >= 150 && line_right_deg <= 210)) || 
                     ((line_right_deg < 30 || line_right_deg > 330) && (line_left_deg >= 150 && line_left_deg <= 210)) 
                    ))
            {
                if(line_data[17] == 0)
                {
                    line_trace_mode = 1;
                }
            }

            int left_speed  =  abs(ball_deg - line_central_deg[3]) <= 75 ? 95 : 50;
            int right_speed =  abs(ball_deg - line_central_deg[2]) <= 75 ? 95 : 50;
            int stop_range  =   ball_distance <= 60 ? 5 : 10;

            if((line_right_deg >= 150 && line_right_deg <= 210))
            {
                right_speed = 20;
            }
            if(line_left_deg >= 150 && line_left_deg <= 210)
            {
                left_speed = 20;
            }

            if(cam_deg_value <= 150)
            {
                motor_move(270, 50);
            }
            else if(cam_deg_value >= 210)
            {
                motor_move(90, 50);
            }
            else
            {
                if(line_trace_mode == 0)
                {
                    if(line_central_deg[0] < stop_range)
                    {
                        if(line_central_deg[0] + stop_range < ball_deg && ball_deg < line_central_deg[1] - stop_range)
                        {
                            if(line_central_deg[0] < line_left_deg && line_left_deg < line_central_deg[1])
                            {
                                motor_move(line_left_deg, left_speed);
                            }
                            else
                            {
                                motor_move(line_right_deg, right_speed);
                            }
                        }
                        else if(ball_deg < line_central_deg[0] - stop_range + 360 && line_central_deg[1] + stop_range < ball_deg)
                        {
                            if(line_left_deg < line_central_deg[0] || line_central_deg[1] < line_left_deg)
                            {
                                motor_move(line_left_deg, left_speed);
                            }
                            else
                            {
                                motor_move(line_right_deg, right_speed);
                            }
                        }
                        else 
                        {
                            motor_move(0, 0);
                        }
                    }
                    else if(line_central_deg[1] > 360 - stop_range)
                    {
                        if(line_central_deg[0] + stop_range < ball_deg && ball_deg < line_central_deg[1] - stop_range)
                        {
                            if(line_central_deg[0] < line_left_deg && line_left_deg < line_central_deg[1])
                            {
                                motor_move(line_left_deg, left_speed);
                            }
                            else
                            {
                                motor_move(line_right_deg, right_speed);
                            }
                        }
                        else if(ball_deg < line_central_deg[0] - stop_range && line_central_deg[1] + stop_range - 360 < ball_deg)
                        {
                            if(line_left_deg < line_central_deg[0] || line_central_deg[1] < line_left_deg)
                            {
                                motor_move(line_left_deg, left_speed);
                            }
                            else
                            {
                                motor_move(line_right_deg, right_speed);
                            }
                        }
                        else 
                        {
                            motor_move(0, 0);
                        }
                    }
                    else
                    {
                        if(line_central_deg[0] + stop_range < ball_deg && ball_deg < line_central_deg[1] - stop_range)
                        {
                            if(line_central_deg[0] < line_left_deg && line_left_deg < line_central_deg[1])
                            {
                                motor_move(line_left_deg, left_speed);
                            }
                            else
                            {
                                motor_move(line_right_deg, right_speed);
                            }
                        }
                        else if(ball_deg < line_central_deg[0] - stop_range || line_central_deg[1] + stop_range < ball_deg)
                        {
                            if(line_left_deg < line_central_deg[0] || line_central_deg[1] < line_left_deg)
                            {
                                motor_move(line_left_deg, left_speed);
                            }
                            else
                            {
                                motor_move(line_right_deg, right_speed);
                            }
                        }
                        else 
                        {
                            motor_move(0, 0);
                        }
                    }
                }
                else if(line_trace_mode == 1)
                {
                    motor_move(0, 50);

                    if(line_data[17] == 1 && 
                        (((line_left_deg < 30 || line_left_deg > 330) && (line_right_deg >= 150 && line_right_deg <= 210)) || 
                        ((line_right_deg < 30 || line_right_deg > 330) && (line_left_deg >= 150 && line_left_deg <= 210)) ))
                    {
                        line_trace_mode = 0;
                    }
                }
            }
        }
        else
        {
            defender_mode = 1;
        }

        if(ball_distance > 60 && !(ball_deg >= 60 && ball_deg <= 300))
        {
            attack_change_wait.start();

            if(attack_change_wait.get_value() >= 5000)
            {
                motor_move(0, 50);

                if(!is_line_detected())
                {
                    defender_mode = 4;
                    reset_line_process();
                    attack_change.reset();
                    attack_change.start();
                }
            }
        }
        else
        {
            attack_change_wait.reset();
        }
        
        if(get_raw_line_deg() != -1)
        {
            def_previous_line_deg = get_raw_line_deg();
        }

        Serial.print(def_previous_line_deg);
    }

    //ライン上に戻るモード
    else if(defender_mode == 1)
    {
        motor_move(def_previous_line_deg, 50);

        Serial.println(def_previous_line_deg);

        if(is_engelline_detected())
        {
            line_trace_mode = 0;
            defender_mode = 0;
        }
    }
    //アタッカーからに戻るモード
    else if(defender_mode == 2)
    {
        if(cam_mode)
        {
            if(cam_deg_value != -1)
            {
                motor_move(cam_deg_value, 55);
            }
            else
            {
                motor_move(180, 75);
            }
        }
        else
        {
            motor_move(180, 50);
        }

        if(is_engelline_detected() && cam_deg_value != -1)
        {
            line_trace_mode = 0;
            defender_mode = 0;
        }
    }
    //アタッカーにするモード
    else if(defender_mode == 3)
    {
        motor_move(0, 50);

        if(is_engelline_detected())
        {
            line_trace_mode = 0;
            defender_mode = 0;
        }
    }
    else if(defender_mode == 4)
    {
        if(cam_mode == true)
        {
            attacker_process_cam(at_cam_deg, at_cam_dis, cam_deg_value, cam_dis_value, 70, 0);
        }
        else
        {
            attacker_process(80);
        }

        if(attack_change.get_value() >= 1000)
        {
            defender_mode = 2;
            attack_change.reset();
            attack_change.start();
        }
    }

    Serial.print(", ");
    Serial.println(defender_mode);
}