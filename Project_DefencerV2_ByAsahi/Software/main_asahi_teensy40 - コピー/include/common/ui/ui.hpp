#include <Arduino.h>

#include "common/button.hpp"

#include "common/ui/bitmaps.hpp"

#include "running/normal_running_gyropid.hpp"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

class Cursor{
public:
    Timer cursor_anime;

    void tick()
    {
        cursor_anime.tick();
    }

    void show(int x, int y, int w, int h)
    {
        cursor_anime.start();

        if(cursor_anime.get_value() <= 500)
        {
            display.fillRect(x, y, w, h, SSD1306_WHITE);
        }
        else if(cursor_anime.get_value() >= 1000)
        {
            cursor_anime.reset();
        }
    }

    void show_string(int x, int y, const char* str, int size)
    {
        cursor_anime.start();

        if(cursor_anime.get_value() <= 500)
        {
            display.setCursor(x, y);
            display.setTextSize(size);
            display.println(str);
        }
        else if(cursor_anime.get_value() >= 1000)
        {
            cursor_anime.reset();
        }
    }

    void show_string(int x, int y, int str, int size)
    {
        cursor_anime.start();

        if(cursor_anime.get_value() <= 500)
        {
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(x, y);
            display.setTextSize(size);
            display.println(str);
        }
        else if(cursor_anime.get_value() >= 1000)
        {
            cursor_anime.reset();
        }
    }
};

Button btEnter(30, Button_Type::BUTTON_PULLDOWN);
Button btLeft(31, Button_Type::BUTTON_PULLDOWN);
Button btRight(32, Button_Type::BUTTON_PULLDOWN);

Bitmaps::LeftArrow left_arrow;
Bitmaps::RightArrow right_arrow;
Bitmaps::Back back;
Bitmaps::Program_icon program_icon;
Bitmaps::Sensors_icon sensors_icon;
Bitmaps::Settings_icon settings_icon;
Bitmaps::Artemis_logo artemis_logo;

Cursor setting_cursor;

//ボール中心設定関連
int Ball_Center_Zone_Mode = 0;

bool is_running = false;

typedef enum {
    RUNNING,
    SENSORS,
    TESTS,
    SETTINGS,
} TOP_PAGETYPE;

typedef enum {
    BACK_FROM_RUNNING = 0,
    YELLOW = 1,
    BLUE = 2,
    GYROONLY_YELLOW = 3,
    GYROONLY_BLUE = 4,

    BACK_FROM_SENSORS = 10,
    BALL = 11,
    LINE = 12,
    GYRO = 13,
    CAMERA_YELLOW = 14,
    CAMERA_BLUE = 15,
    CAMERA_COURT = 16,

    BACK_FROM_TESTS = 20,
    BALL_TEST = 21,
    LINE_TEST = 22,
    PID_GYRO_TEST = 23,
    PID_YELLOW_TEST = 24,
    PID_BLUE_TEST = 25,
    KICK_TEST = 26,

    BACK_FROM_SETTINGS = 30,
    SET_BALL_CENTER_ZONE = 31,
} MIDDLE_PAGETYPE;

typedef enum{
    NORMAL_YELLOW,
    NORMAL_BLUE,
    NORMAL_GYROONLY,
    TESTS_BALL,
    TESTS_LINE,
    TESTS_PID_GYRO,
    TESTS_PID_YELLOW,
    TESTS_PID_BLUE,
} RUNNING_TYPE;

int now_length = 0;

TOP_PAGETYPE toppage = TOP_PAGETYPE::RUNNING;
MIDDLE_PAGETYPE middle_page = MIDDLE_PAGETYPE::BACK_FROM_RUNNING;

RUNNING_TYPE running_type = RUNNING_TYPE::NORMAL_GYROONLY;

void ui_setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  display.clearDisplay();
  display.drawBitmap(0, 0, artemis_logo.logo_bmp, artemis_logo.WIDTH, artemis_logo.HEIGHT, SSD1306_WHITE);
  display.display();

  delay(1000);
}

void ui_main()
{
    btEnter.loop();
    btLeft.loop();
    btRight.loop();

    setting_cursor.tick();
    
    if(btRight.isPushed())
    {
        if(now_length == 1)
        {
            toppage = (TOP_PAGETYPE)(((int)toppage + 1) % 4);
        }
        else if(now_length == 2)
        {
            if(toppage == TOP_PAGETYPE::RUNNING)
            {
                middle_page = (MIDDLE_PAGETYPE)(((int)middle_page + 1) % 5);
            }
            else if(toppage == TOP_PAGETYPE::SENSORS)
            {
                middle_page = (MIDDLE_PAGETYPE)((((int)middle_page - 10 + 1) % 7) + 10);
            }
            else if(toppage == TOP_PAGETYPE::TESTS)
            {
                middle_page = (MIDDLE_PAGETYPE)((((int)middle_page - 20 + 1) % 7) + 20);
            }
            else if(toppage == TOP_PAGETYPE::SETTINGS)
            {
                middle_page = (MIDDLE_PAGETYPE)((((int)middle_page - 30 + 1) % 2) + 30);
            }
        }
    }
    if(btLeft.isPushed())
    {
        if(now_length == 1)
        {
            toppage = (TOP_PAGETYPE)(((int)toppage + 3) % 4);
        }
        else if(now_length == 2)
        {
            if(toppage == TOP_PAGETYPE::RUNNING)
            {
                middle_page = (MIDDLE_PAGETYPE)(((int)middle_page + 4) % 5);
            }
            else if(toppage == TOP_PAGETYPE::SENSORS)
            {
                middle_page = (MIDDLE_PAGETYPE)((((int)middle_page - 10 + 6) % 7) + 10);
            }
            else if(toppage == TOP_PAGETYPE::TESTS)
            {
                middle_page = (MIDDLE_PAGETYPE)((((int)middle_page - 20 + 6) % 7) + 20);
            }
            else if(toppage == TOP_PAGETYPE::SETTINGS)
            {
                middle_page = (MIDDLE_PAGETYPE)((((int)middle_page - 30 + 1) % 2) + 30);
            }
        }
    }

    display.clearDisplay();

    if(now_length == 1)
    {
        switch(toppage)
        {
            case TOP_PAGETYPE::RUNNING:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(64 - 12 * 5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("ASAHI PROGRAM V1.0.0");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(36, 30);
                    display.setTextSize(2);
                    display.println("PROGRAM");
                
                    display.drawBitmap(5, 26, program_icon.logo_bmp, program_icon.WIDTH, program_icon.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length++;
                        middle_page = MIDDLE_PAGETYPE::BACK_FROM_RUNNING;
                    }
                }
                break;

            case TOP_PAGETYPE::SENSORS:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(64 - 12 * 5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("ASAHI PROGRAM V1.0.0");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(36, 30);
                    display.setTextSize(2);
                    display.println("SENSORS");
                
                    display.drawBitmap(8, 26, sensors_icon.logo_bmp, sensors_icon.WIDTH, sensors_icon.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length++;
                        middle_page = MIDDLE_PAGETYPE::BACK_FROM_SENSORS;
                    }
                }
                break;

            case TOP_PAGETYPE::TESTS:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(64 - 12 * 5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("ASAHI PROGRAM V1.0.0");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(36, 30);
                    display.setTextSize(2);
                    display.println("TESTS");
                
                    display.drawBitmap(5, 26, program_icon.logo_bmp, program_icon.WIDTH, program_icon.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length++;
                        middle_page = MIDDLE_PAGETYPE::BACK_FROM_TESTS;
                    }
                }
                break;

            case TOP_PAGETYPE::SETTINGS:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(64 - 12 * 5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("ASAHI PROGRAM V1.0.0");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(31, 30);
                    display.setTextSize(2);
                    display.println("SETTINGS");
                
                    display.drawBitmap(3, 26, settings_icon.logo_bmp, settings_icon.WIDTH, settings_icon.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length++;
                        middle_page = MIDDLE_PAGETYPE::BACK_FROM_SETTINGS;
                    }
                }
                break;
        }
    }
    else if(now_length == 2)
    {
        switch(middle_page)
        {
            case MIDDLE_PAGETYPE::BACK_FROM_RUNNING:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("BACK TO MAIN");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("1/5");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(54, 32);
                    display.setTextSize(2);
                    display.println("BACK");
                    
                    display.drawBitmap(22, 28, back.logo_bmp, back.WIDTH, back.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length--;
                        toppage = TOP_PAGETYPE::RUNNING;
                    }
                }
                break;

            case MIDDLE_PAGETYPE::YELLOW:
                {
                    if(is_running)
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("RUNNING!");
                    }
                    else
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("YELLOW");  

                        display.setCursor(54, 48);
                        display.setTextSize(2);
                        display.println(Gyro_Deg);
                    }
                    
                    if(btEnter.isPushed())
                    {
                        running_type = RUNNING_TYPE::NORMAL_YELLOW;
                        is_running = !is_running;
                    }
                }
                break;
                
            case MIDDLE_PAGETYPE::BLUE:
                {
                    if(is_running)
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("RUNNING!");
                    }
                    else
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("BLUE");
                        
                        display.setCursor(54, 48);
                        display.setTextSize(2);
                        display.println(Gyro_Deg);
                    }
                    if(btEnter.isPushed())
                    {
                        running_type = RUNNING_TYPE::NORMAL_BLUE;
                        is_running = !is_running;
                    }
                }
                break;

            case MIDDLE_PAGETYPE::GYROONLY_YELLOW:
                {
                    if(is_running)
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("RUNNING!");
                    }
                    else
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("GYRO YELLOW");
                        
                        display.setCursor(54, 48);
                        display.setTextSize(2);
                        display.println(Gyro_Deg);
                    }

                    if(btEnter.isPushed())
                    {
                        running_type = RUNNING_TYPE::NORMAL_GYROONLY;
                        is_running = !is_running;
                    }
                }
                break;


            case MIDDLE_PAGETYPE::GYROONLY_BLUE:
                {
                    if(is_running)
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("RUNNING!");
                    }
                    else
                    {
                        display.setCursor(10, 25);
                        display.setTextSize(2);
                        display.println("GYRO BLUE");
                        
                        display.setCursor(54, 48);
                        display.setTextSize(2);
                        display.println(Gyro_Deg);
                    }

                    if(btEnter.isPushed())
                    {
                        running_type = RUNNING_TYPE::NORMAL_GYROONLY;
                        is_running = !is_running;
                    }
                }
                break;

            case MIDDLE_PAGETYPE::BACK_FROM_SENSORS:
                {                
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("BACK TO MAIN");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("1/7");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(54, 32);
                    display.setTextSize(2);
                    display.println("BACK");
                    
                    display.drawBitmap(22, 28, back.logo_bmp, back.WIDTH, back.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length--;
                        toppage = TOP_PAGETYPE::SENSORS;
                    }
                }
                break;

            case MIDDLE_PAGETYPE::BALL:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("BALL CHECK");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("2/7");

                    display.setTextColor(SSD1306_WHITE);

                    display.drawCircle(36, 42, 18, SSD1306_WHITE);

                    if(Ball_Deg != -1)
                    {
                        int distance = min((int)((100 - (Ball_Distance)) / 100.0 * 30), 18);
                        int pointx = 36 + cos(radians(Ball_Deg - 90)) * distance;
                        int pointy = 42 + sin(radians(Ball_Deg - 90)) * distance;
                        display.drawLine(36, 42, pointx, pointy, SSD1306_WHITE);
                        display.drawCircle(pointx, pointy, 2, SSD1306_WHITE);
                    }

                    display.setCursor(70, 32);
                    display.setTextSize(1);
                    display.println("Deg:" + String((int)Ball_Deg));
                    display.setCursor(70, 47);
                    display.setTextSize(1);
                    display.println("Dis:" + String((int)Ball_Distance));

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);
                }
                break;

            case MIDDLE_PAGETYPE::LINE:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("LINE CHECK");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("3/7");

                    display.setTextColor(SSD1306_WHITE);

                    display.drawCircle(36, 42, 18, SSD1306_WHITE);

                    if(Line_Deg != -1)
                    {
                        int pointx = 36 + cos(radians(Line_Deg - 90)) * 18;
                        int pointy = 42 + sin(radians(Line_Deg - 90)) * 18;
                        display.drawLine(36, 42, pointx, pointy, SSD1306_WHITE);

                        int pointx_l = 36 + cos(radians(Line_Deg - 90 - 20)) * 12;
                        int pointy_l = 42 + sin(radians(Line_Deg - 90 - 20)) * 12;
                        display.drawLine(pointx, pointy, pointx_l, pointy_l, SSD1306_WHITE);

                        int pointx_r = 36 + cos(radians(Line_Deg - 90 + 20)) * 12;
                        int pointy_r = 42 + sin(radians(Line_Deg - 90 + 20)) * 12;
                        display.drawLine(pointx, pointy, pointx_r, pointy_r, SSD1306_WHITE);
                    }

                    display.setCursor(70, 24);
                    display.setTextSize(1);
                    display.println("Deg:" + String((int)Line_Deg));
                    display.setCursor(70, 34);
                    display.setTextSize(1);
                    display.println("Old:" + String((int)engelline.Line_Deg_First));
                    display.setCursor(70, 44);
                    display.setTextSize(1);
                    display.println("Dis:" + String((int)Line_Distance));
                    display.setCursor(70, 54);
                    display.setTextSize(1);
                    display.println("Mode:" + String((int)engelline.bIsLineHalfOut == 0 ? "N" : "Out"));
                    
                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);
                }
                break;

            case MIDDLE_PAGETYPE::GYRO:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("GYRO CHECK");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("4/7");

                    display.setTextColor(SSD1306_WHITE);

                    display.drawCircle(36, 42, 18, SSD1306_WHITE);

                    int pointx = 36 + cos(radians((Gyro_Deg / 1024.0 * 360.0) + 90)) * 18;
                    int pointy = 42 + sin(radians((Gyro_Deg / 1024.0 * 360.0) + 90)) * 18;
                    display.drawLine(36, 42, pointx, pointy, SSD1306_WHITE);
                    display.drawCircle(pointx, pointy, 2, SSD1306_WHITE);

                    display.setCursor(70, 36);
                    display.setTextSize(1);
                    display.println("Deg:" + String((int)Gyro_Deg));
                    
                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);
                }
                break;

            case MIDDLE_PAGETYPE::CAMERA_YELLOW:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("CAMERA YELLOW");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("5/7");

                    display.setTextColor(SSD1306_WHITE);

                    display.drawCircle(36, 42, 18, SSD1306_WHITE);
                    int pointx = 36 + cos(radians((Gyro_Deg / 1024.0 * 360.0) + 90)) * 18;
                    int pointy = 42 + sin(radians((Gyro_Deg / 1024.0 * 360.0) + 90)) * 18;
                    display.drawLine(36, 42, pointx, pointy, SSD1306_WHITE);
                    display.drawCircle(pointx, pointy, 2, SSD1306_WHITE);

                    display.setCursor(70, 32);
                    display.setTextSize(1);
                    display.println("Deg:" + String((int)Yellow_Goal_Deg));
                    display.setCursor(70, 47);
                    display.setTextSize(1);
                    display.println("Dis:" + String((int)Yellow_Goal_Distance));
                    
                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);
                }
                break;

            case MIDDLE_PAGETYPE::CAMERA_BLUE:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("CAMERA BLUE");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("6/7");

                    display.setTextColor(SSD1306_WHITE);

                    display.drawCircle(36, 42, 18, SSD1306_WHITE);
                    int pointx = 36 + cos(radians((Gyro_Deg / 1024.0 * 360.0) + 90)) * 18;
                    int pointy = 42 + sin(radians((Gyro_Deg / 1024.0 * 360.0) + 90)) * 18;
                    display.drawLine(36, 42, pointx, pointy, SSD1306_WHITE);
                    display.drawCircle(pointx, pointy, 2, SSD1306_WHITE);

                    display.setCursor(70, 32);
                    display.setTextSize(1);
                    display.println("Deg:" + String((int)Blue_Goal_Deg));
                    display.setCursor(70, 47);
                    display.setTextSize(1);
                    display.println("Dis:" + String((int)Blue_Goal_Distance));
                    
                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);
                }
                break;

            case MIDDLE_PAGETYPE::CAMERA_COURT:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("CAMERA GREEN");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("7/7");

                    display.setTextColor(SSD1306_WHITE);

                    display.drawCircle(36, 42, 18, SSD1306_WHITE);

                    display.setCursor(70, 32);
                    display.setTextSize(1);
                    display.println("Deg:" + String((int)Court_Deg));
                    display.setCursor(70, 47);
                    display.setTextSize(1);
                    display.println("Dis:" + String((int)Court_Distance));

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);
                }
                break;

            case MIDDLE_PAGETYPE::BACK_FROM_TESTS:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("BACK TO MAIN");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("1/6");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(54, 32);
                    display.setTextSize(2);
                    display.println("BACK");
                    
                    display.drawBitmap(22, 28, back.logo_bmp, back.WIDTH, back.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length--;
                        toppage = TOP_PAGETYPE::TESTS;
                    }
                }
                break;

            case MIDDLE_PAGETYPE::KICK_TEST:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("KICK TEST");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("6/6");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(54, 32);
                    display.setTextSize(2);
                    display.println("KICK TEST");

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    kicker.tick();

                    if(btEnter.isPushed())
                    {
                        kicker.kick();
                    }
                }
                break;

            case MIDDLE_PAGETYPE::BACK_FROM_SETTINGS:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("BACK TO MAIN");

                    display.setCursor(128 - 20, 4);
                    display.setTextSize(1);
                    display.println("1/2");

                    display.setTextColor(SSD1306_WHITE);

                    display.setCursor(54, 32);
                    display.setTextSize(2);
                    display.println("BACK");
                    
                    display.drawBitmap(22, 28, back.logo_bmp, back.WIDTH, back.HEIGHT, SSD1306_WHITE);

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);

                    if(btEnter.isPushed())
                    {
                        now_length--;
                        toppage = TOP_PAGETYPE::SETTINGS;
                    }
                }
                break;

            case SET_BALL_CENTER_ZONE:
                {
                    display.fillRect(0, 0, 128, 15, SSD1306_WHITE);

                    display.setCursor(5, 4);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_BLACK);
                    display.println("SET BALL CENTER ZONE");
                    
                    display.setCursor(35, 50);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_WHITE);
                    display.println("Deg:" + String(Ball_Deg));

                    if(Ball_Center_Zone_Mode == 0)
                    {
                        display.setCursor(30, 25);
                        display.setTextSize(2);
                        display.setTextColor(SSD1306_WHITE);
                        display.println(Ball_Center_Left);

                        display.setCursor(70, 25);
                        display.setTextSize(2);
                        display.setTextColor(SSD1306_WHITE);
                        display.println(Ball_Center_Right);

                        if(btEnter.isPushed())
                        {
                            Ball_Center_Zone_Mode = 1;
                        }
                    }
                    else if(Ball_Center_Zone_Mode == 1)
                    {
                        Ball_Center_Left = Ball_Deg;

                        setting_cursor.show_string(30, 25, Ball_Center_Left, 2);

                        display.setCursor(70, 25);
                        display.setTextSize(2);
                        display.setTextColor(SSD1306_WHITE);
                        display.println(Ball_Center_Right);

                        if(btEnter.isPushed())
                        {
                            Ball_Center_Zone_Mode = 2;
                            Ball_Center_Left = Ball_Deg;
                        }
                    }
                    else if(Ball_Center_Zone_Mode == 2)
                    {
                        Ball_Center_Right = Ball_Deg;

                        display.setCursor(30, 25);
                        display.setTextSize(2);
                        display.setTextColor(SSD1306_WHITE);
                        display.println(Ball_Center_Left);

                        setting_cursor.show_string(70, 25, Ball_Center_Right, 2);

                        if(btEnter.isPushed())
                        {
                            Ball_Center_Zone_Mode = 0;
                        }
                    }

                    display.drawBitmap(3, 55, left_arrow.logo_bmp, left_arrow.WIDTH, left_arrow.HEIGHT, SSD1306_WHITE);
                    display.drawBitmap(117, 55, right_arrow.logo_bmp, right_arrow.WIDTH, right_arrow.HEIGHT, SSD1306_WHITE);
                }
                break;
        }
    }

    display.display();

    //初回起動防止用
    if(btEnter.isPushed())
    {
        if(now_length == 0)
        {
            now_length = 1;
        }
    }
}