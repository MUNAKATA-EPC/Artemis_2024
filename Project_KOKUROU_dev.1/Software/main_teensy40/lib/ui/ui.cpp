#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Servo.h>

#include "sensors_variables.hpp"
#include "button.hpp"
#include "attacker.hpp"
#include "l3gd20h.hpp"
#include "kicker.hpp"
#include "timer.hpp"
#include "line.hpp"
#include "ui.hpp"

#define MENU_COUNT 10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//extern部
extern Button btEnter;
extern Button btLeft;
extern Button btRight;

extern Kicker kicker;
extern Servo dribbler;

extern bool is_able_to_run;
extern int type_of_run;

//データ送信部
Timer send_timer;
int next_send_time;

//選択用変数
typedef enum{
    RUN = 0,
    SENSOR,
    TEST,
    TOTAL
} FIRST_FLOOR;

typedef enum{
    //RUN
    RUN_BACK = 0,
    RUN_GYRO,
    RUN_CAM_YELLOW,
    RUN_CAM_BLUE,
    RUN_TOTAL,

    //SENSOR
    SENSOR_BACK = 10,
    SENSOR_BALL,
    SENSOR_LINE,
    SENSOR_GYRO,
    SENSOR_HOLD,
    SENSOR_CAM_YELLOW,
    SENSOR_CAM_BLUE,
    SENSOR_TOTAL,

    //TEST
    TEST_BACK = 20,
    TEST_KICKER,
    TEST_DRIBBLER,
    TEST_LINE,
    TEST_TOTAL
} SECOND_FLOOR;

int now_floor;
int now_select_type;
int first_floor_type;

bool is_dribbler_roll;
int dribbler_speed = 0;

bool is_selected()
{
    return btEnter.is_pushed();
}

void ui_init()
{
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);

    display.setRotation(2);

    display.clearDisplay();
    display.println("setup now...");
    display.display();

    now_floor = 1;
    now_select_type = 0;
    first_floor_type = 0;

    send_timer.start();
}

void ui_loop()
{
    send_timer.tick();
    
    if(!is_able_to_run)
    {
        if(btRight.is_pushed())
        {
            tone(6, 4000, 15);

            if(now_floor == 1)
            {
                now_select_type = (now_select_type + 1) % FIRST_FLOOR::TOTAL;
            }
            else if(now_floor == 2)
            {
                if(first_floor_type == 0)
                {
                    now_select_type = (now_select_type + 1) % (int)(SECOND_FLOOR::RUN_TOTAL);
                } 
                else if(first_floor_type == 1)
                {
                    now_select_type = ((now_select_type - 10) + 1) % (int)((SECOND_FLOOR::SENSOR_TOTAL) - 10) + 10;
                } 
                else if(first_floor_type == 2)
                {
                    now_select_type = ((now_select_type - 20) + 1) % (int)((SECOND_FLOOR::TEST_TOTAL) - 20) + 20;
                }
            }
        }

        if(btLeft.is_pushed())
        {
            tone(6, 4000, 15);

            if(now_floor == 1)
            {
                now_select_type = (now_select_type - 1 + FIRST_FLOOR::TOTAL) % FIRST_FLOOR::TOTAL;
            }
            else if(now_floor == 2)
            {
                if(first_floor_type == 0)
                {
                    now_select_type = (now_select_type - 1 + (int)(SECOND_FLOOR::RUN_TOTAL)) % (int)(SECOND_FLOOR::RUN_TOTAL);
                } 
                else if(first_floor_type == 1)
                {
                    now_select_type = ((now_select_type - 10) - 1 + (int)((SECOND_FLOOR::SENSOR_TOTAL) - 10)) % (int)((SECOND_FLOOR::SENSOR_TOTAL) - 10) + 10;
                } 
                else if(first_floor_type == 2)
                {
                    now_select_type = ((now_select_type - 20) - 1 + (int)((SECOND_FLOOR::TEST_TOTAL) - 20)) % (int)((SECOND_FLOOR::TEST_TOTAL) - 20) + 20;
                }
            }
        }
    }

    if(btEnter.is_pushed())
    {
        tone(6, 4500, 15);
    }
    
    display.clearDisplay();

    if(now_floor == 1)
    {
        if(now_select_type == (int)(FIRST_FLOOR::RUN))
        {
            display.setTextSize(2);
            display.setCursor(0, 25);
            display.println("RUN");

            if(is_selected())
            {
                now_floor = 2;
                now_select_type = 0;
                first_floor_type = 0;
            }
        }
        else if(now_select_type == (int)(FIRST_FLOOR::SENSOR))
        {
            display.setTextSize(2);
            display.setCursor(0, 25);
            display.println("SENSOR");

            if(is_selected())
            {
                now_floor = 2;
                now_select_type = 10;
                first_floor_type = 1;
            }
        }
        else if(now_select_type == (int)(FIRST_FLOOR::TEST))
        {
            display.setTextSize(2);
            display.setCursor(0, 25);
            display.println("TEST");

            if(is_selected())
            {
                now_floor = 2;
                now_select_type = 20;
                first_floor_type = 2;
            }
        }
    }
    else if(now_floor == 2)
    {
        //RUN
        switch(now_select_type)
        {
            case (int)(SECOND_FLOOR::RUN_BACK):
                {
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.println("BACK");

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 1/4 >");

                    if(is_selected())
                    {
                        now_floor = 1;
                        now_select_type = 0;
                        first_floor_type = 0;
                    }
                }
                break;

            case (int)(SECOND_FLOOR::RUN_GYRO):
                {
                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 2/4 >");

                    if(is_able_to_run)
                    {
                        display.setTextSize(2);
                        display.setCursor(0, 25);
                        display.println("RUN:GYRO");
                    }
                    else
                    {
                        display.setTextSize(2);
                        display.setCursor(0, 25);
                        display.println("AT:GYRO");
                    }

                    type_of_run = 0;

                    if(is_selected())
                    {
                        attacker_init();   
                        is_able_to_run = !is_able_to_run;
                    }
                }
                break;

            case (int)(SECOND_FLOOR::RUN_CAM_YELLOW):
                {
                    if(is_able_to_run)
                    {
                        display.setTextSize(2);
                        display.setCursor(0, 25);
                        display.println("RUN:YELLOW");
                    }
                    else
                    {
                        display.setTextSize(2);
                        display.setCursor(0, 25);
                        display.println("AT:YELLOW");
                    }

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 3/4 >");

                    type_of_run = 1;

                    if(is_selected())
                    {
                        attacker_init(); 
                        is_able_to_run = !is_able_to_run;
                    }
                }
                break;

            case (int)(SECOND_FLOOR::RUN_CAM_BLUE):
                {
                    if(is_able_to_run)
                    {
                        display.setTextSize(2);
                        display.setCursor(0, 25);
                        display.println("RUN:BLUE");
                    }
                    else
                    {
                        display.setTextSize(2);
                        display.setCursor(0, 25);
                        display.println("AT:BLUE");
                    }

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 4/4 >");

                    type_of_run = 2;

                    if(is_selected())
                    {
                        attacker_init(); 
                        is_able_to_run = !is_able_to_run;
                    }
                }
                break;



            case (int)(SECOND_FLOOR::SENSOR_BACK):
                {
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.println("BACK");

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 1/7 >");

                    if(is_selected())
                    {
                        now_floor = 1;
                        now_select_type = 1;
                        first_floor_type = 1;
                    }
                }
                break;

            case (int)(SECOND_FLOOR::SENSOR_BALL):
                {
                    display.setTextSize(1);
                    display.setCursor(0, 10);
                    display.println("ball");

                    display.setCursor(0, 25);
                    display.println("deg:" + String(ball_deg));
                    display.setCursor(0, 35);
                    display.println("dis:" + String(ball_distance));

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 2/7 >");
                }
                break;

            case (int)(SECOND_FLOOR::SENSOR_LINE):
                {
                    display.setTextSize(1);
                    display.setCursor(0, 10);
                    display.println("line");

                    display.setCursor(0, 25);
                    display.println("detected:" + String(line_value));

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 3/7 >");
                }
                break;

            case (int)(SECOND_FLOOR::SENSOR_GYRO):
                {
                    display.setTextSize(1);
                    display.setCursor(0, 10);
                    display.println("gyro");

                    display.setCursor(0, 25);
                    display.println("deg:" + String(gyro_data));
                    display.setCursor(0, 35);
                    display.println("cal:" + String(gyro_calibration_rtrim_data));

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 4/7 >");
                }
                break;

            case (int)(SECOND_FLOOR::SENSOR_HOLD):
                {
                    display.setTextSize(1);
                    display.setCursor(0, 10);
                    display.println("hold");

                    display.setCursor(0, 25);
                    display.println("val:" + String(hold_value));

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 5/7 >");
                }
                break;

            case (int)(SECOND_FLOOR::SENSOR_CAM_YELLOW):
                {
                    display.setTextSize(1);
                    display.setCursor(0, 10);
                    display.println("cam yellow");

                    display.setCursor(0, 25);
                    display.println("deg:" + String(yellow_goal_deg));
                    display.setCursor(0, 35);
                    display.println("dis:" + String(yellow_goal_distance));

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 6/7 >");
                }
                break;

            case (int)(SECOND_FLOOR::SENSOR_CAM_BLUE):
                {
                    display.setTextSize(1);
                    display.setCursor(0, 10);
                    display.println("cam blue");

                    display.setCursor(0, 25);
                    display.println("deg:" + String(blue_goal_deg));
                    display.setCursor(0, 35);
                    display.println("dis:" + String(blue_goal_distance));

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 7/7 >");
                }
                break;



            case (int)(SECOND_FLOOR::TEST_BACK):
                {
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.println("BACK");

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 1/4 >");

                    if(is_selected())
                    {
                        now_floor = 1;
                        now_select_type = 2;
                        first_floor_type = 2;
                    }
                }
                break;

            case (int)(SECOND_FLOOR::TEST_KICKER):
                {
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.println("KICK TEST");

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 2/4 >");

                    if(is_selected())
                    {
                        kicker.kick();
                    }
                }
                break;

            case (int)(SECOND_FLOOR::TEST_DRIBBLER):
                {
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.println("DRIBBLER TEST");

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 3/4 >");

                    display.setCursor(0, 40);
                    display.println("speed:" + String(dribbler_speed));

                    if(is_selected())
                    {
                        dribbler_speed = (dribbler_speed + 100) % 700;
                        
                        if(dribbler_speed == 0)
                        {
                            is_dribbler_roll = false;
                        }
                        else
                        {
                            is_dribbler_roll = true;
                        }
                    }

                    if(is_dribbler_roll)
                    {
                        dribbler.writeMicroseconds(dribbler_speed + 1000);
                    }
                    else
                    {
                        dribbler.writeMicroseconds(1000);
                    }
                }
                break;

            case (int)(SECOND_FLOOR::TEST_LINE):
                {
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.println("LINE TEST");

                    display.setTextSize(1);
                    display.setCursor(0, 35);
                    display.println("deg:" + String(line_value));

                    if(is_able_to_run)
                    {
                        display.setCursor(0, 45);
                        display.println("running!");
                    }

                    display.setTextSize(1);
                    display.setCursor(43, 48);
                    display.println("< 4/4 >");

                    type_of_run = 3;

                    if(is_selected())
                    {
                        is_able_to_run = !is_able_to_run;
                    }
                }
                break;
        }
    }

    if(send_timer.get_value() >= next_send_time)
    {
        display.display();

        next_send_time = send_timer.get_value() + 50;
    }
}