#ifndef _ENGELLINE_H
#define _ENGELLINE_H

#include <Arduino.h>
#include "../motor.hpp"
#include "../timer.hpp"
#include "../serial_receive.hpp"

class EngelLine{
private:
    int _speed;
    int _deg;

public:
    Timer Line_Timer;
    Timer InLine_Timer;
    Timer AfterLine_Wait;

    int Line_Deg_First;
    int Line_DegBefore;
    int Line_DegBefore_WithoutUndetected;
    int Line_Deg_start;
    int Line_DistanceBefore;
    bool bLine_Detected;
    bool bIsLineHalfOut;
    int HalfOutTime;
    int MaybeHalfOutTime;
    bool bIsMaybeHalfOut;
    bool bIsAbleToDisturb;
    
    void engelline_init()
    {
        AfterLine_Wait.set_value(9999);
    }

    void engelline_main(int speed)
    {
        int line_speed = speed;


        Line_Timer.tick();
        AfterLine_Wait.tick();
        InLine_Timer.tick();
        
        if(Line_Deg != -1)
        {
            if(!bLine_Detected)
            {
                Line_Timer.start();
                Line_Deg_First = Line_Deg;
                Line_Deg_start = Line_Deg;
                bLine_Detected = true;
                AfterLine_Wait.stop();
                AfterLine_Wait.reset();
            }
        }

        if(Line_Timer.isticking())
        {
            {
                if(bIsMaybeHalfOut && Line_Timer.get_value() >= MaybeHalfOutTime + 30)
                {
                    Line_DistanceBefore = 1;
                    bIsMaybeHalfOut = false;
                }

                if(bIsAbleToDisturb)
                {
                    HalfOutTime = Line_Timer.get_value();
                    bIsAbleToDisturb = false;
                }


                //おかしいから元の角度で戻してあげる
                if(Line_Deg == -1)
                {
                    if(!bIsLineHalfOut)
                    {
                        InLine_Timer.stop();
                        InLine_Timer.reset();
                        Line_Timer.stop();
                        Line_Timer.reset();
                        AfterLine_Wait.start();
                        bLine_Detected = false;
                        bIsMaybeHalfOut = false;
                    }
                    else
                    {
                        _deg = Line_Deg_First;
                        _speed = line_speed;

                        //緊急回避用
                        if(Line_Timer.get_value() >= HalfOutTime + 100)
                        {
                            bIsLineHalfOut = false;
                        }
                    }
                }
                else
                {
                    if(Line_DegBefore == -1 && !bIsLineHalfOut && !bIsMaybeHalfOut)
                    {
                        //Line_Deg_start = Line_Deg;
                        Line_Deg_First = Line_Deg;
                    }

                    //FIXME: オーバーラインの処理角度に用いろ値を変える必要あり。
                    if(Line_DegBefore == -1 && Line_Deg >= Line_Deg_First - 30 && Line_Deg <= Line_Deg_First + 30)
                    {
                        //前回反応していなくて、今回反応しているとき、一度角度チェックを入れる
                        //もし初めの角度と+-30度くらいずれてたら、2本目のラインを踏んだと仮定して、元の角度で動く
                        //それ以外であれば、普通にライン制御をする。
                        _deg = Line_Deg_First;
                        _speed = line_speed;
                    }
                    else
                    {
                        int diff_deg = 75;

                        int diff_deg_update = 60;

                        if((Line_Deg == 271 || Line_Deg == 91) && !(Line_DegBefore == -1))
                        {
                            Line_Distance = 4;
                            Line_Deg = Line_Deg_First;
                            _deg = Line_Deg_First;
                            _speed = line_speed;
                        }
                        else if((Line_Deg == 1 || Line_Deg == 181) && !(Line_DegBefore == -1))
                        {
                            Line_Distance = 4;
                            Line_Deg = Line_Deg_First;
                            _deg = Line_Deg_First;
                            _speed = line_speed;
                        }
                        else
                        {
                            if(Line_Deg_First >= 180)
                            {
                                //条件角度の大きさによって条件式を変化させる。
                                if(Line_Deg_First + 180 - diff_deg >= 360)
                                {
                                    if(Line_Deg >= (Line_Deg_First + 180 - diff_deg) % 360 && Line_Deg <= Line_Deg_First - 180 + diff_deg)
                                    {
                                        _deg = Line_Deg_First;
                                        _speed = line_speed;
                                        bIsLineHalfOut = true;
                                        bIsMaybeHalfOut = false;
                                        HalfOutTime = Line_Timer.get_value();
                                        bIsAbleToDisturb = true;
                                    }
                                    else
                                    {
                                        bIsAbleToDisturb = false;
                                        bIsLineHalfOut = false;
                                        _deg = Line_Deg;
                                        _speed = line_speed;

                                        if(Line_Deg_First + diff_deg_update >= 360)
                                        {
                                            if((Line_Deg <= (Line_Deg_First + diff_deg_update) % 360 || Line_Deg >= Line_Deg_First - diff_deg_update))
                                            {
                                                Line_Deg_First = Line_Deg;
                                            }
                                        }
                                        else
                                        {
                                            if(Line_Deg >= (Line_Deg_First - diff_deg_update) && Line_Deg <= Line_Deg_First + diff_deg_update)
                                            {
                                                Line_Deg_First = Line_Deg;
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(Line_Deg >= Line_Deg_First + 180 - diff_deg || Line_Deg <= Line_Deg_First - 180 + diff_deg)
                                    {
                                        _deg = Line_Deg_First;
                                        _speed = line_speed;
                                        bIsLineHalfOut = true;
                                        bIsMaybeHalfOut = false;
                                        HalfOutTime = Line_Timer.get_value();
                                        bIsAbleToDisturb = true;
                                    }
                                    else
                                    {
                                        bIsAbleToDisturb = false;
                                        bIsLineHalfOut = false;                       
                                        _deg = Line_Deg;
                                        _speed = line_speed;

                                        if(Line_Deg_First + diff_deg_update >= 360)
                                        {
                                            if((Line_Deg <= (Line_Deg_First + diff_deg_update) % 360 || Line_Deg >= Line_Deg_First - diff_deg_update))
                                            {
                                                Line_Deg_First = Line_Deg;
                                            }
                                        }
                                        else
                                        {
                                            if(Line_Deg >= (Line_Deg_First - diff_deg_update) && Line_Deg <= Line_Deg_First + diff_deg_update)
                                            {
                                                Line_Deg_First = Line_Deg;
                                            }
                                        }
                                    }
                                }
                            }

                            else
                            {
                                //条件角度の大きさによって条件式を変化させる。
                                if(Line_Deg_First + 180 + diff_deg >= 360)
                                {
                                    if(Line_Deg >= Line_Deg_First + 180 - diff_deg || Line_Deg <= (Line_Deg_First + 180 + diff_deg) % 360)
                                    {
                                        _deg = Line_Deg_First;
                                        _speed = line_speed;
                                        bIsLineHalfOut = true;
                                        bIsMaybeHalfOut = false;
                                        HalfOutTime = Line_Timer.get_value();
                                        bIsAbleToDisturb = true;
                                    }
                                    else
                                    {
                                        bIsAbleToDisturb = false;
                                        bIsLineHalfOut = false;
                                        _deg = Line_Deg;
                                        _speed = line_speed;

                                        if(Line_Timer.get_value() >= 80)
                                        {
                                            if(Line_Deg_First - diff_deg_update <= 0)
                                            {
                                                if((Line_Deg >= (Line_Deg_First - diff_deg_update + 360) || Line_Deg <= (Line_Deg_First + diff_deg_update)))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                            else
                                            {
                                                if((Line_Deg >= (Line_Deg_First - diff_deg_update) && Line_Deg <= Line_Deg_First + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(Line_Deg >= Line_Deg_First + 180 - diff_deg && Line_Deg <= Line_Deg_First + 180 + diff_deg)
                                    {
                                        _deg = Line_Deg_First;
                                        _speed = line_speed;
                                        bIsLineHalfOut = true;
                                        bIsMaybeHalfOut = false;
                                        HalfOutTime = Line_Timer.get_value();
                                        bIsAbleToDisturb = true;
                                    }
                                    else
                                    {
                                        bIsAbleToDisturb = false;
                                        bIsLineHalfOut = false;
                                        _deg = Line_Deg;
                                        _speed = line_speed;

                                        if(Line_Timer.get_value() >= 80)
                                        {
                                            if(Line_Deg_First - diff_deg_update <= 0)
                                            {
                                                if((Line_Deg >= (Line_Deg_First - diff_deg_update + 360) || Line_Deg <= (Line_Deg_First + diff_deg_update)))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                            else
                                            {
                                                if((Line_Deg >= (Line_Deg_First - diff_deg_update) && Line_Deg <= Line_Deg_First + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if(Line_Timer.get_value() <= 2000)
                    {
                        _deg = Line_Deg_start;
                    }
                    InLine_Timer.stop();
                    InLine_Timer.reset();
                }
            }
        }


        if(Line_Timer.get_value() <= 50)
        {
            //_speed = 100;
        }

        Line_DistanceBefore = Line_Distance; 
        Line_DegBefore = Line_Deg;
        Line_DegBefore_WithoutUndetected = Line_Deg != -1 ? Line_Deg : Line_DegBefore_WithoutUndetected;
    }

    //ラインアウト防止の移動をしているかの真偽値を返します。
    bool IsLineMovement()
    {
        return Line_Timer.isticking();
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

EngelLine engelline;

#endif