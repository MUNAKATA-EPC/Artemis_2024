#ifndef _ENGELLINE_H
#define _ENGELLINE_H

#include <Arduino.h>

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

    void engelline_main(int speed){

        int line_speed = speed - 10;

        if(Line_Timer.get_value() <= 50)
        {
            line_speed = 90;
        }

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
            if(InLine_Timer.get_value() >= 30)
            {
                InLine_Timer.stop();
                InLine_Timer.reset();
                Line_Timer.stop();
                Line_Timer.reset();
                AfterLine_Wait.start();
                bLine_Detected = false;
                bIsMaybeHalfOut = false;
                bIsLineHalfOut = false;
            }
            else
            {
                if(bIsAbleToDisturb)
                {
                    HalfOutTime = Line_Timer.get_value();
                    bIsAbleToDisturb = false;
                }

                //緊急回避用
                if(Line_Timer.get_value() >= HalfOutTime + 100 && bIsLineHalfOut && !bIsMaybeHalfOut && Line_Deg == -1 && Line_DistanceBefore == 0)
                {
                    bIsLineHalfOut = false;
                }

                if(Line_Deg == -1 && bIsLineHalfOut == true)
                {
                    speed = speed + 10;
                }

                //おかしいから元の角度で戻してあげる
                if(Line_Deg == -1)
                {
                    if(!bIsLineHalfOut)
                    {
                        if(bIsMaybeHalfOut)
                        {
                            _deg = Line_Deg_First;
                            _speed = line_speed;
                            if(Line_Timer.get_value() >= MaybeHalfOutTime + 50)
                            {
                                Line_DistanceBefore = 1;
                                bIsMaybeHalfOut = false;
                            }
                        }
                        else
                        {
                            if(Line_DistanceBefore >= 1)
                            {
                                bIsMaybeHalfOut = true;
                                MaybeHalfOutTime = Line_Timer.get_value();
                                _deg = Line_Deg_First;
                                _speed = line_speed;
                            }
                            else if(Line_DistanceBefore != -1)
                            {   
                                Line_Distance = 0.5;
                                _deg = Line_Deg_First;
                                _speed = line_speed;
                                InLine_Timer.start();
                            }
                            else
                            {
                                _deg = Line_Deg_First;
                                _speed = line_speed;
                                InLine_Timer.start();
                            }    
                        }
                    }
                    else
                    {
                        _deg = Line_Deg_First;
                        _speed = line_speed;
                    }
                }
                else
                {
                    if(Line_DegBefore == -1 && !bIsLineHalfOut && !bIsMaybeHalfOut)
                    {
                        Line_Deg_start = Line_Deg;
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
                        int diff_deg = 80;
                        int diff_deg_update = 180 - (200 / 2);

                        if((Line_Deg == 271 || Line_Deg == 91) && (!(Line_Deg_First >= 60 && Line_Deg_First <= 120) || !(Line_Deg_First >= 240 && Line_Deg_First <= 300) || Line_DistanceBefore >= 2))
                        {
                            Line_Distance = 4;
                            Line_Deg = Line_Deg_First;
                            _deg = Line_Deg_First;
                            _speed = line_speed;
                        }
                        else if((Line_Deg == 1 || Line_Deg == 181) && ((Line_Deg_First <= 150 && Line_Deg_First >= 30) || (Line_Deg_First <= 330 && Line_Deg_First >= 210) || Line_DistanceBefore >= 2))
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
                                        if(Line_Timer.get_value() >= 80)
                                        {
                                            if(Line_Deg_First + 180 - diff_deg_update >= 360)
                                            {
                                                if(!(Line_Deg >= (Line_Deg_First + 180 - diff_deg_update) % 360 && Line_Deg <= Line_Deg_First - 180 + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                            else
                                            {
                                                if(!(Line_Deg >= (Line_Deg_First + 180 - diff_deg_update) || Line_Deg <= Line_Deg_First - 180 + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                        }
                                        _deg = Line_Deg;
                                        _speed = line_speed;
                                        bIsLineHalfOut = false;
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
                                        if(Line_Timer.get_value() >= 80)
                                        {
                                            if(Line_Deg_First + 180 - diff_deg_update >= 360)
                                            {
                                                if(!(Line_Deg >= (Line_Deg_First + 180 - diff_deg_update) % 360 && Line_Deg <= Line_Deg_First - 180 + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                            else
                                            {
                                                if(!(Line_Deg >= (Line_Deg_First + 180 - diff_deg_update) || Line_Deg <= Line_Deg_First - 180 + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                        }                              
                                        _deg = Line_Deg;
                                        _speed = line_speed;
                                        bIsLineHalfOut = false;
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
                                        if(Line_Timer.get_value() >= 80)
                                        {
                                            if(Line_Deg_First + 180 + diff_deg_update >= 360)
                                            {
                                                if(!(Line_Deg >= Line_Deg_First + 180 - diff_deg_update || Line_Deg <= (Line_Deg_First + 180 + diff_deg_update) % 360))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                            else
                                            {
                                                if(!(Line_Deg >= (Line_Deg_First + 180 - diff_deg_update) && Line_Deg <= Line_Deg_First - 180 + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                        }
                                        _deg = Line_Deg;
                                        _speed = line_speed;
                                        bIsLineHalfOut = false;
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
                                        if(Line_Timer.get_value() >= 80)
                                        {
                                            if(Line_Deg_First + 180 + diff_deg_update >= 360)
                                            {
                                                if(!(Line_Deg >= Line_Deg_First + 180 - diff_deg_update || Line_Deg <= (Line_Deg_First + 180 + diff_deg_update) % 360))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                            else
                                            {
                                                if(!(Line_Deg >= (Line_Deg_First + 180 - diff_deg_update) && Line_Deg <= Line_Deg_First - 180 + diff_deg_update))
                                                {
                                                    Line_Deg_First = Line_Deg;
                                                }
                                            }
                                        }
                                        _deg = Line_Deg;
                                        _speed = line_speed;
                                        bIsLineHalfOut = false;
                                    }
                                }
                            }
                        }
                    }
                    InLine_Timer.stop();
                    InLine_Timer.reset();
                }
            }
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