#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#include "ui.hpp"

#include "sensors_variables.hpp"

#include "button.hpp"
#include "timer.hpp"
#include "kicker.hpp"

#include "engelline.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Button btEnter;
Button btLeft;
Button btRight;

extern int line_first_deg;

extern Kicker kicker;

enum RUN_MODE run_mode;

//データ送信部
Timer send_timer;
int next_send_time = 0;

extern bool is_able_to_run;

int now_floor = 1;
int now_select_mode = 0;
int parent_floor;

typedef enum{
    RUN,
    SENSOR,
    TEST,
    TOTAL
} FIRST_FLOOR;

typedef enum{
    RUN_BACK = 0,
    RUN_AT_GYRO,
    RUN_AT_YELLOW,
    RUN_AT_BLUE,
    RUN_AT_GYRO_SLOW,
    RUN_AT_YELLOW_SLOW,
    RUN_AT_BLUE_SLOW,
    RUN_DF_GYRO,
    RUN_DF_YELLOW,
    RUN_DF_BLUE,
    RUN_TOTAL,

    SENSOR_BACK = 20,
    SENSOR_BALL,
    SENSOR_LINE,
    SENSOR_GYRO,
    SENSOR_YELLOW,
    SENSOR_BLUE,
    SENSOR_TOUCH,
    SENSOR_ALLY_MODE,
    SENSOR_TOTAL,

    TEST_BACK = 30,
    TEST_KICKER,
    TEST_KICKER_DISCHARGE,
    TEST_DRIBBLER,
    TEST_LINE,
    TEST_TOTAL,
} SECOND_FLOOR;

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
    display.setCursor(0, 50);
    display.println("setup now...");
    display.display();

    btEnter.init(6, Button::Button_Value_Type::PULLDOWN);
    btLeft.init(9, Button::Button_Value_Type::PULLDOWN);
    btRight.init(10, Button::Button_Value_Type::PULLDOWN);

    send_timer.start();
}

void ui_process()
{
    send_timer.tick();

    btEnter.loop();
    btLeft.loop();
    btRight.loop();

    if(is_selected())
    {
        tone(4, 4500, 15);
    }
    
    //if(!is_able_to_run && (parent_floor == (int)FIRST_FLOOR::TEST ? true : false))
    {
        if(btLeft.is_pushed())
        {
            tone(4, 4000, 15);

            if(now_floor == 1)
            {
                now_select_mode = (now_select_mode - 1 + FIRST_FLOOR::TOTAL) % FIRST_FLOOR::TOTAL;
            }
            else
            {
                if(parent_floor == (int)FIRST_FLOOR::RUN)
                {
                    now_select_mode = (now_select_mode - 1 + SECOND_FLOOR::RUN_TOTAL) % SECOND_FLOOR::RUN_TOTAL;
                }
                else if(parent_floor == (int)FIRST_FLOOR::SENSOR)
                {
                    now_select_mode = (now_select_mode - 1 - 20 + (SECOND_FLOOR::SENSOR_TOTAL - 20)) % (SECOND_FLOOR::SENSOR_TOTAL - 20) + 20;
                }
                else if(parent_floor == (int)FIRST_FLOOR::TEST)
                {
                    now_select_mode = (now_select_mode - 1 - 30 + (SECOND_FLOOR::TEST_TOTAL - 30)) % (SECOND_FLOOR::TEST_TOTAL - 30) + 30;
                }
            }
        }
        if(btRight.is_pushed())
        {
            tone(4, 4000, 15);

            if(now_floor == 1)
            {
                now_select_mode = (now_select_mode + 1) % FIRST_FLOOR::TOTAL;
            }
            else
            {
                if(parent_floor == (int)FIRST_FLOOR::RUN)
                {
                    now_select_mode = (now_select_mode + 1) % SECOND_FLOOR::RUN_TOTAL;
                }
                else if(parent_floor == (int)FIRST_FLOOR::SENSOR)
                {
                    now_select_mode = (now_select_mode + 1 - 20 + (SECOND_FLOOR::SENSOR_TOTAL - 20)) % (SECOND_FLOOR::SENSOR_TOTAL - 20) + 20;
                }
                else if(parent_floor == (int)FIRST_FLOOR::TEST)
                {
                    now_select_mode = (now_select_mode + 1 - 30 + (SECOND_FLOOR::TEST_TOTAL - 30)) % (SECOND_FLOOR::TEST_TOTAL - 30) + 30;
                }
            }
        }
    }

    display.clearDisplay();


    if(now_floor == 1)
    {
        if(now_select_mode == (int)FIRST_FLOOR::RUN)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("RUN");

            if(is_selected())
            {
                now_floor = 2;
                parent_floor = (int)FIRST_FLOOR::RUN;
                now_select_mode = (int)SECOND_FLOOR::RUN_BACK;
            }
        }
        else if(now_select_mode == (int)FIRST_FLOOR::SENSOR)
        { 
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("SENSOR");

            if(is_selected())
            {
                now_floor = 2;
                parent_floor = (int)FIRST_FLOOR::SENSOR;
                now_select_mode = (int)SECOND_FLOOR::SENSOR_BACK;
            }
        }
        else if(now_select_mode == (int)FIRST_FLOOR::TEST)
        { 
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("TEST");

            if(is_selected())
            {
                now_floor = 2;
                parent_floor = (int)FIRST_FLOOR::TEST;
                now_select_mode = (int)SECOND_FLOOR::TEST_BACK;
            }
        }
    }
    else if(now_floor == 2)
    {
        //RUN CATEGORY7
        if(now_select_mode == (int)SECOND_FLOOR::RUN_BACK)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("BACK");

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::RUN;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_AT_GYRO)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "AT:GYRO" : "RUN:GYRO");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::ATTACKER_GYRO;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_AT_YELLOW)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "AT:YELLOW" : "RUN:YELLOW");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::ATTACKER_YELLOW;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_AT_BLUE)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "AT:BLUE" : "RUN:BLUE");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::ATTACKER_BLUE;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_AT_GYRO_SLOW)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "ATS:GYRO" : "RUN:GYRO");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::ATTACKER_GYRO_SLOW;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_AT_YELLOW_SLOW)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "ATS:YELLOW" : "RUN:YELLOW");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::ATTACKER_YELLOW_SLOW;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_AT_BLUE_SLOW)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "ATS:BLUE" : "RUN:BLUE");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::ATTACKER_BLUE_SLOW;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_DF_GYRO)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "DF:GYRO" : "RUN:GYRO");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::DEFENDER_GYRO;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_DF_YELLOW)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "DF:YELLOW" : "RUN:YELLOW");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::DEFENDER_YELLOW;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::RUN_DF_BLUE)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(!is_able_to_run ? "DF:BLUE" : "RUN:BLUE");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::DEFENDER_BLUE;
            }
        }

        //SENSOR CATEGORY
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_BACK)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("BACK");

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_BALL)
        {
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("ball");
            display.setCursor(0, 30);
            display.println("deg:" + String(ball_deg));
            display.setCursor(0, 40);
            display.println("dis:" + String(ball_distance));

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_LINE)
        {
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("line");
            display.setCursor(0, 30);
            display.println("deg:" + String(get_line_deg()));//get_count_of_detcted_sensors
            display.setCursor(64, 30);
            display.println("ct:" + String(get_count_of_detcted_sensors()));
            display.setCursor(0, 40);
            display.println("deg:" + String(line_first_deg));
            display.setCursor(0, 50);
            display.println("halfout:" + String(is_line_halfout()));

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_GYRO)
        {
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("gyro");
            display.setCursor(0, 30);
            display.println("deg:" + String(gyro_deg));//get_count_of_detcted_sensors
            display.setCursor(64, 30);

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_YELLOW)
        {
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("yellow");
            display.setCursor(0, 30);
            display.println("deg:" + String(cam_yellow_deg));
            display.println("dis:" + String(cam_yellow_dis));

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_BLUE)
        {
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("blue");
            display.setCursor(0, 30);
            display.println("deg:" + String(cam_blue_deg));
            display.println("dis:" + String(cam_blue_dis));

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_TOUCH)
        {
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("hold sensor");
            display.setCursor(0, 30);
            display.println("hold:" + String(hold_value));

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::SENSOR_ALLY_MODE)
        {
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("ally_mode");
            display.setCursor(0, 30);
            display.println("mode:" + String(ally_mode == 0 ? "near_ball" : ally_mode == 1 ? "far_ball" : ally_mode == 2 ? "holding" : ally_mode == 3 ? "lost" : "no-work"));

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::SENSOR;
            }
        }

        //TEST CATEGORY
        else if(now_select_mode == (int)SECOND_FLOOR::TEST_BACK)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("BACK");

            if(is_selected())
            {
                now_floor = 1;
                parent_floor = -1;
                now_select_mode = (int)FIRST_FLOOR::TEST;
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::TEST_KICKER)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("KICK TEST");

            kicker.loop();

            if(is_selected())
            {
                kicker.kick();
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::TEST_KICKER_DISCHARGE)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println("KICK DISCHARGE");

            if(btEnter.is_pushing())
            {
                kicker.kick();
            }
        }
        else if(now_select_mode == (int)SECOND_FLOOR::TEST_DRIBBLER)
        {
            display.setTextSize(2);
            display.setCursor(0, 35);
            display.println(is_able_to_run ? "ROLLING!" : "DRIBBLER TEST");

            if(is_selected())
            {
                is_able_to_run = !is_able_to_run;
                run_mode = RUN_MODE::DRIBBLER_TEST;
            }
        }
    }   
        
    if(send_timer.get_value() >= next_send_time)
    {
        display.display();

        next_send_time = send_timer.get_value() + 100;
    }
}