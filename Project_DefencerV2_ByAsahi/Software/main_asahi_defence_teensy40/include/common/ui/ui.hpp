#include <Arduino.h>

#include "common/button.hpp"
#include "common/timer.hpp"

#include "common/ui/bitmaps.hpp"

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
    }
    if(btLeft.isPushed())
    {
    }
    if(btEnter.isPushed())
    {
        is_running = !is_running;
    }

    display.clearDisplay();

    if(is_running)
    {
        display.setCursor(5, 5);
        display.setTextSize(2);
        display.println("running");
    }
    else
    {
        display.setCursor(5, 5);
        display.setTextSize(2);
        display.println("push enter");
    }

    display.setTextSize(1);
    display.setCursor(5, 25);
    display.println(String("Cam_Dir:") + String(Yellow_Goal_Deg));
    display.setCursor(5, 35);
    display.println(String("Cam_Dis:") + String(Yellow_Goal_Distance));

    display.display();
}