#ifndef _BALL_TRACKING_H
#define _BALL_TRACKING_H

#include "common/vector.hpp"
#include "common/serial_receive.hpp"
#include "common/timer.hpp"
#include "common/config.hpp"
#include "common/modules/dribbler.hpp"
#include "common/modules/kicker.hpp"

class Ball_Tracking_WithGyro{
private:
    Timer BallInFront;
    Timer BallCatchTime;

    int FrontMove_Mode = 0;

    float BallCenterSide_SpeedPer = 0.90;
    float Ball_Near_SpeedPer = 0.85;
    float Ball_Near_SpeedPer_Fast = 1.0;
    float BallCenterTimer_SpeedPer = 0.90;
    float BallCenterTimer_SpeedPer_BeforeKick = 0.900;
    float BallCenterTimer_SpeedPer_Kicking = 1.00;
    float Ball_Near_Catch = 0.90;
    float Ball_Near_Catch_Delay = 0.95;

    int _speed;
    int _deg;

    typedef enum 
    {
        Ball_Tracking = 0,
        Ball_Bringing = 1,
    } Ball_Tracking_Mode;

    Ball_Tracking_Mode btm = Ball_Tracking_Mode::Ball_Tracking;
public:

    bool IsAbleToTracking()
    {
        return Ball_Deg != -1;
    }

    void ball_tracking_main(int origin_speed, int camera_dir = 255)
    {
        BallInFront.tick();
        BallCatchTime.tick();
        
        if(Ball_Deg == -1)
        {
            _deg = 0;
            _speed = 0;
            Dribbler.writeMicroseconds(1000);
        }
        else
        {
            if(btm == Ball_Tracking_Mode::Ball_Tracking)
            {
                if(HoldSensor == 1)
                {
                    kicker.kick(200);
                    btm = Ball_Tracking_Mode::Ball_Bringing;
                }
                if(Ball_Deg >= 340 || Ball_Deg <= 20)
                {
                    if(HoldSensor == 1)
                    {
                        _deg = 0;
                        kicker.kick(100);
                        btm = Ball_Tracking_Mode::Ball_Bringing;
                    }
                    else
                    {
                        Dribbler.writeMicroseconds(1030);
                        if(Ball_Distance >= 45)
                        {          
                            _deg = 0;
                            _speed = origin_speed * BallCenterTimer_SpeedPer_BeforeKick;
                        }
                        else
                        {
                            _deg = 0;
                            _speed = origin_speed * BallCenterTimer_SpeedPer;
                        }
                    }
                    BallInFront.start();
                }
                else
                {
                    BallInFront.stop();
                    BallInFront.reset();

                    FrontMove_Mode = FrontMove_Mode == 0 ? 1 : 0;

                    if(Ball_Deg <= 90)
                    {
                        Vector vec_to_ball = Vector(radians(Ball_Deg), max((35 - max((Ball_Distance - 50), 0)), 0));
                        Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg + 90), 14);
                        vec_to_ball.add(vec_to_tan_line_ball);
                        _deg = vec_to_ball.get_deg();
                        _speed = Ball_Distance >= 70 ? Ball_Deg <= 60 ? origin_speed * BallCenterSide_SpeedPer : origin_speed * Ball_Near_SpeedPer : 
                            Ball_Distance >= 65 ? origin_speed * Ball_Near_SpeedPer_Fast : origin_speed;
                    }
                    else if(Ball_Deg <= 180)
                    {
                        Vector vec_to_ball = Vector(radians(Ball_Deg), max((30 - max((Ball_Distance - 50), 0)), 0));
                        Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg + 90), 8);
                        vec_to_ball.add(vec_to_tan_line_ball);
                        _deg = vec_to_ball.get_deg();
                        _speed = Ball_Distance >= 70 ? Ball_Deg <= 40 ? origin_speed * BallCenterSide_SpeedPer : origin_speed * Ball_Near_SpeedPer : 
                            true ? origin_speed * Ball_Near_SpeedPer_Fast : origin_speed;
                    }
                    else if(Ball_Deg <= 270)
                    {
                        Vector vec_to_ball = Vector(radians(Ball_Deg), max((30 - max((Ball_Distance - 50), 0)), 0));
                        Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg - 90), 8);
                        vec_to_ball.add(vec_to_tan_line_ball);
                        _deg = vec_to_ball.get_deg();
                        _speed = Ball_Distance >= 70 ? (Ball_Deg <= 40 ? origin_speed * BallCenterSide_SpeedPer : origin_speed * Ball_Near_SpeedPer) : 
                            true ? origin_speed * Ball_Near_SpeedPer_Fast : origin_speed;
                    }
                    else
                    {
                        Vector vec_to_ball = Vector(radians(Ball_Deg), max((35 - max((Ball_Distance - 50), 0)), 0));
                        Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg - 90), 12);
                        vec_to_ball.add(vec_to_tan_line_ball);
                        _deg = vec_to_ball.get_deg();
                        _speed = Ball_Distance >= 70 ? Ball_Deg >= 300 ? origin_speed * BallCenterSide_SpeedPer : origin_speed * Ball_Near_SpeedPer : 
                            Ball_Distance >= 65 ? origin_speed * Ball_Near_SpeedPer_Fast : origin_speed;
                    }

                    if(Ball_Deg <= 40 || Ball_Deg >= 320)
                    {
                        Dribbler.writeMicroseconds(1040);
                    }
                    else
                    {
                        Dribbler.writeMicroseconds(1000);
                    }

                    if((Ball_Deg >= 310 || Ball_Deg <= 45) && Ball_Distance >= 60 && HoldSensor == 0)
                    {
                        BallCatchTime.start();

                        if(BallCatchTime.get_value() <= 2000)
                        {
                            _speed = _speed * Ball_Near_Catch;
                        }
                        else if(BallCatchTime.get_value() >= 4000)
                        {
                            BallCatchTime.stop();
                            BallCatchTime.reset();
                        }
                        else
                        {
                            _speed = _speed * Ball_Near_Catch_Delay;
                        }
                    }
                }
            }
            else if(btm == Ball_Tracking_Mode::Ball_Bringing)
            {
                if(kicker.get_kicking() == false)
                    kicker.kick(400);

                _deg = 0;
                _speed = origin_speed * BallCenterTimer_SpeedPer_Kicking;
                Dribbler.writeMicroseconds(1120);

                if(!(Ball_Deg <= 40 || Ball_Deg >= 320) || HoldSensor == 0)
                {
                    btm = Ball_Tracking;
                    BallCatchTime.reset();
                    BallCatchTime.stop();
                }
            }

        }
    }

    int get_speed()
    {
        return _speed;
    }

    int get_degrees()
    {
        return _deg;
    }
};



class Ball_Tracking_WithCamera{
private:
    Timer BallInFront;

    float BallCenterSide_SpeedPer = 0.60;
    float Ball_Near_SpeedPer = 0.70;
    float Ball_Near_SpeedPer_Fast = 0.85;
    float BallCenterTimer_SpeedPer = 0.75;
    float BallCenterTimer_SpeedPer_BeforeKick = 0.80;
    float BallCenterTimer_SpeedPer_Kicking = 0.90;

    int _speed;
    int _deg;

public:
    bool IsAbleToTracking()
    {
        return Ball_Deg != -1;
    }

    /// @brief you can track ball with using camera data.
    /// @param camera_data camera_data([0]:goal_deg [1]:goal_distance)
    void ball_tracking_main(int speed, int camera_data[])
    {         
        //kicker.tick();

        if(HoldSensor == 1)
        {
            if(Gyro_Deg >= 660)
            {
                _deg = 270;
                Dribbler.writeMicroseconds(1050);
                _speed = speed * 0.65;
            }
            else if(Gyro_Deg <= 360)
            {
                _deg = 90;
                Dribbler.writeMicroseconds(1050);
                _speed = speed * 0.65;
            }
            else
            {
                if(camera_data[1] >= 120)
                {
                    kicker.kick(100);
                    _deg = 0;
                }
                else
                {
                    kicker.kick(100);
                    _deg = Gyro_Deg <= 412 ? 30 : Gyro_Deg >= 612 ? 330 : 0;
                }
                _speed = speed * BallCenterTimer_SpeedPer_Kicking;
                Dribbler.writeMicroseconds(1040);
            }
        }
        else if(Ball_Deg  != -1)
        {
            if(((Ball_Deg <= Ball_Center_Right + 2 || Ball_Deg >= Ball_Center_Left - 2) && Ball_Distance >= 60) ||
                ((Ball_Deg >= 340 || Ball_Deg <= 20) && Ball_Distance < 60))
            {
                {
                    Dribbler.writeMicroseconds(1040);
                    if(Ball_Distance >= 50)
                    {          
                        _deg = 0;
                        _speed = speed * BallCenterTimer_SpeedPer_BeforeKick;
                    }
                    else
                    {
                        _deg = 0;
                        _speed = speed * BallCenterTimer_SpeedPer;
                    }
                }
                BallInFront.start();
            }
            else
            {
                Dribbler.writeMicroseconds(1000);

                BallInFront.stop();
                BallInFront.reset();

                if(Ball_Deg <= 90)
                {
                    Vector vec_to_ball = Vector(radians(Ball_Deg), max((42 - max((Ball_Distance - 50), 0)), 0));
                    Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg + 90), 22);
                    vec_to_ball.add(vec_to_tan_line_ball);
                    _deg = vec_to_ball.get_deg();
                    _speed = Ball_Distance >= 70 ? Ball_Deg <= 40 ? speed * BallCenterSide_SpeedPer : speed * Ball_Near_SpeedPer : 
                        Ball_Distance >= 55 ? speed * Ball_Near_SpeedPer_Fast : speed;
                }
                else if(Ball_Deg <= 180)
                {
                    Vector vec_to_ball = Vector(radians(Ball_Deg), max((30 - max((Ball_Distance - 50), 0)), 0));
                    Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg + 90), 23);
                    vec_to_ball.add(vec_to_tan_line_ball);
                    _deg = vec_to_ball.get_deg();
                    _speed = Ball_Distance >= 78 ? Ball_Deg <= 40 ? speed * BallCenterSide_SpeedPer : speed * Ball_Near_SpeedPer : 
                        Ball_Distance >= 65 ? speed * Ball_Near_SpeedPer_Fast : speed;
                }
                else if(Ball_Deg <= 270)
                {
                    Vector vec_to_ball = Vector(radians(Ball_Deg), max((30 - max((Ball_Distance - 50), 0)), 0));
                    Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg - 90), 23);
                    vec_to_ball.add(vec_to_tan_line_ball);
                    _deg = vec_to_ball.get_deg();
                    _speed = Ball_Distance >= 78 ? (Ball_Deg <= 40 ? speed * BallCenterSide_SpeedPer : speed * Ball_Near_SpeedPer) : 
                        Ball_Distance >= 65 ? speed * Ball_Near_SpeedPer_Fast : speed;
                }
                else
                {
                    Vector vec_to_ball = Vector(radians(Ball_Deg), max((35 - max((Ball_Distance - 50), 0)), 0));
                    Vector vec_to_tan_line_ball = Vector(radians(Ball_Deg - 90), 26);
                    vec_to_ball.add(vec_to_tan_line_ball);
                    _deg = vec_to_ball.get_deg();
                    _speed = Ball_Distance >= 78 ? Ball_Deg >= 320 ? speed * BallCenterSide_SpeedPer : speed * Ball_Near_SpeedPer : 
                        Ball_Distance >= 65 ? speed * Ball_Near_SpeedPer_Fast : speed;
                }

                if((Ball_Deg >= 310 || Ball_Deg <= 45) && Ball_Distance >= 60 && HoldSensor == 0)
                {
                    _speed = _speed * 0.9;
                }
            }
        }
        else
        {
            Dribbler.writeMicroseconds(1000);
        }
    }

    int get_speed()
    {
        return _speed;
    }

    int get_degrees()
    {
        return _deg;
    }
};

Ball_Tracking_WithGyro ball_tracking_gyro;
Ball_Tracking_WithCamera ball_tracking_camera;

#endif