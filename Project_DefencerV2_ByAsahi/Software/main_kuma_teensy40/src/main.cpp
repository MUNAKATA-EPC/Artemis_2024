#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include "DSR1202.h"


#include "timer.hpp"
#include "button.hpp"

#include "serial_receive.hpp"
#include "motor.hpp"
#include "engelline.hpp"


#define ROBO_SPEED 50
#define court_half_Y 109
#define court_half_B 121
#define goal_near_Y 64 //前に出るゴールとの距離（黄色）
#define goal_near_B 80 //　　　　　　　　　　　（青色）
#define goal_kado_Y 104 //ラインの端っこの距離（黄色）
#define goal_kado_B 64 //                   （青色）
#define goal_kado_Y_R 33
#define goal_kado_Y_L 33

Button btEnter(30, Button_Type::BUTTON_PULLUP);
Button btLeft(31, Button_Type::BUTTON_PULLUP);
Button btRight(32, Button_Type::BUTTON_PULLUP);

Timer Line_Timer;
Timer InLine_Timer;
Timer AfterLine_Wait;
Timer Ball_timer;

Timer LineTrace_FakeTimer;

int Line_Deg_First;
int LineDegBefore;
bool bLine_Detected;
bool bIsLineHalfOut;
double move_deg;
double move_speed;
int cam_date[2];
int goal_near;
int goal_kado_R;
int goal_kado_L;
int old_gyro; 
int old_line;
bool line;


bool yellow = 0;
bool isLineTrace;

bool Line_Moving;

bool start_program = false;

// now_mode : 現在のロボットの状態を表す
// 0 : ライントレース
// 1 : ボール追跡
// 2 : ゴール帰着
int now_mode = 0;

bool is_running = false;

void setup() {
  pinMode(30, INPUT_PULLDOWN);
  pinMode(31, INPUT_PULLDOWN);
  pinMode(32, INPUT_PULLDOWN);

  serials_init();
  motor_init();

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  Serial.begin(9600);
}

void loop() {
  serials_receive();

  btEnter.loop();
  btLeft.loop();
  btRight.loop();

  Line_Timer.tick();
  AfterLine_Wait.tick();
  InLine_Timer.tick();
  Ball_timer.tick();
  LineTrace_FakeTimer.tick();
  pid_motion.pid_gyro();


  if(btLeft.isPushed())
  {
    yellow = !yellow;
  }

  if(btEnter.isPushed())
  {
    start_program = !start_program;
  }

  if(yellow == 0)
  {    
      
    cam_date[0] = Cam_GoalY_Deg; 
    cam_date[1] = Cam_GoalY_Dis; 
    goal_near = goal_near_Y;
    goal_kado_R = goal_kado_Y_R;
    goal_kado_L = goal_kado_Y_L;
    display.setCursor(10, 10);
    display.println("yellow");
    display.display();
  }
  else
  {
    cam_date[0] = Cam_GoalB_Deg; 
    cam_date[1] = Cam_GoalB_Dis;
    goal_near = goal_near_B;
    //goal_kado = goal_kado_B;
    display.setCursor(10, 10);
    display.println("blue");
    display.display();
  }

  
  if(Line_Deg != -1)
  {
    if(Ball_Deg != -1)
    {
      if((Ball_Deg > 350 || Ball_Deg < 10) || (Ball_Deg > 150 && Ball_Deg < 200))
      {
        move_deg = (-1, 0);
        move_deg = (degrees(move_deg));
        move_deg = move_deg * -2.0;
        move_speed = 0;
      }
      else if(Ball_Deg <= 180)
      {
        move_deg = atan2(-1, 1);
        move_deg = (degrees(move_deg));
        move_deg = move_deg * -2.0;
        move_speed = 50 + (Ball_Deg >= 90 ? (180 - Ball_Deg) / 1.5 : Ball_Deg / 1.5);
      }
      else
      {
        move_deg = atan2(-1, -1);
        move_deg = (degrees(move_deg));
        move_deg = move_deg * -2.0;
        move_speed = 50 + (Ball_Deg >= 270 ? (360 - Ball_Deg) / 1.5 : (Ball_Deg - 180) / 1.5);
      }  
    }
    else
    {
      move_deg = cam_date[0] * 2;
      move_speed = cam_date[1] / 2.0;
    }
  }
  else
  {
    if(Ball_Deg != -1)
    {
      if((Ball_Deg > 355 || Ball_Deg < 5) || (Ball_Deg > 150 && Ball_Deg < 200))
      {
        move_deg = (-1, 0);
        move_deg = (degrees(move_deg));
        move_speed = 0;
      }
      else if(Ball_Deg <= 180)
      {
        move_deg = atan2(-1, 1);
        move_deg = (degrees(move_deg));
        if(Ball_Distance > 0)
        {
          move_speed = Ball_Distance * 1.2;
        }
        move_deg = (move_deg -90) * -1;
      }
      else
      {
        move_deg = atan2(-1, -1);
        move_deg = (degrees(move_deg));
        if(Ball_Distance > 0)
        {
          move_speed = Ball_Distance * 1.2;
        }
        move_deg = (move_deg -90) * -1;

      }  
    }
    else
    {
      move_deg = cam_date[0] * 2;
      move_speed = cam_date[1] / 2.0;
    }  
  }

  if(start_program)
  {
    if(Line_Deg != -1)
    {
      old_line = Line_Deg;
    }

    if(now_mode == 0)  //ライントレースモードの時
    {   
      if(Line_Deg != -1)
      {
        if(Ball_Deg != -1)
          {
            if(cam_date[0] > 115 && cam_date[0] < 130)
            {
              if(Ball_Deg <= 180)
              {
                move(0,0);
              }
              else
              {
                move(move_deg, move_speed);
              }
            }
            else if(cam_date[0] > 39 && cam_date[0] < 81)
            {
              if(Ball_Deg <= 180)
              {
                move(move_deg, move_speed);
              }
              else
              {
                move(0, 0);
              }
            }
            else
            {
              move(move_deg, move_speed);
            }
          }
          else
          {
            if(cam_date[0] > 78 && cam_date[0] < 103)//ゴールの端から端までの角度
            {
              move(0, 0);
            }
            else
            {
              if(cam_date[1] < goal_kado_R)
              {
                if(Line_Deg <= 180)
                {
                  move(315, 55);
                }
                else
                {
                  move(45, 55);
                }
              }
              else
              {
                move(cam_date[0] * 2, cam_date[1] / 2.0);
              }
            }
          }
      }
      else
      {
        if(cam_date[1] < goal_near)
        {
            if(Ball_Deg <= 180)
            {
              move(70, 45);
            }
            else
            {
              move(290, 45);
            }       
        }
        else
        {
          if(cam_date[0] > 78 && cam_date[0] < 103)//ゴールの端から端までの角度
          {
            if(Ball_Deg <= 180)
            {
              move(110, 50);
            }
            else
            {
              move(250, 50);
            }            
          }
          else
          {
            move(cam_date[0] * 2, cam_date[1] / 2.0);
          }
        }
      }
    } 
    else if(now_mode == 1)  //ボール追跡モードの時
    {
      //ライン踏んだらコート内に戻る
      if(Line_Deg != -1)
      {
        move(Line_Deg, 50);
      }
      else
      {
        move(move_deg, 50);        
      }   
      //動く的なsomething

      if(Ball_timer.get_value() >= 3000)
      {
        //ゴール帰着モードに切り替え
        Ball_timer.stop();
        Ball_timer.reset();
        now_mode = 2;
      }
      //多分急にライントレースモードに入ることはないと思うよ
    }
    else if(now_mode == 2)  //ゴール帰着モード
    {
      Ball_timer.stop();
      Ball_timer.reset();

      
      /*if(Line_Deg != -1)  
      {
        move(Cam_Court_Deg, 40);
      }  
      else if(Cam_Court_Deg > 90 && Cam_Court_Deg < 270)
      {
        move(Cam_Court_Deg, 40);
      }
      else if((old_line >= 180 && old_line < 220))
      {
        move(0, 25);
      }
      else
      {
        if(Cam_Court_Dis < 10)
        {
          if(Ball_Deg >= 20 && Ball_Deg <= 140)
          {
            move(130, 40);
          }
          else if(Ball_Deg <= 330 && Ball_Deg >= 220)
          {
            move(230, 40);
          }
          else
          {
            move(180, 40);
          }
        }
        else 
        {
          if(Cam_Court_Deg >= 340 || Cam_Court_Deg <= 20)
          {
            if(Ball_Deg >= 20 && Ball_Deg <= 140)
            {
              move(135, 40);
            }
            else if(Ball_Deg <= 330 && Ball_Deg >= 220)
            {
              move(225, 40);
            }
            else
            {
              move(180, 40);
            }
          }  
          else if(Cam_Court_Deg > 20 && Cam_Court_Deg <= 90)
          {
            move(94, 40);
          }
          else if(Cam_Court_Deg >= 270 && Cam_Court_Deg <= 340)
          {
            move(266, 40);
          }*/
    }   
      
      
      
      
      //ライン踏んだ時はうまい具合にゴール前白線に戻る
      //戻る的なsomething

      if(Line_Deg >= 0 && Line_Deg < 60)
      {
        old_line = -1;
        now_mode = 0;
      }
      //多分急にボール追跡モードに入ることはないと思
    
  }
  else
  {
    motor_power(0, 0, 0, 0);
  }
 
  Serial.print(move_deg);
  Serial.print(" ");
  Serial.print(move_speed);
  Serial.print(" ");
  Serial.print(cam_date[0]);
  Serial.print(" ");
  Serial.print(cam_date[1]);
  Serial.print(" ");
  //Serial.print(goal_kado);
  Serial.print(" ");
  Serial.print(Ball_Distance);
  Serial.print(" ");
  Serial.println(Ball_Deg);

  display.clearDisplay();
  display.setCursor(30, 10);
  display.println(Line_Deg);
  display.setCursor(10, 30);
  display.println(move_deg);
  display.setCursor(100, 10);
  display.println(now_mode);
  display.setCursor(68, 10);
  display.println(Gyro_Deg);
  display.drawLine(80, 40, 80 + cos(Gyro_Deg / 512.0 * 3.14 + 256 - 512) * 20, 40 + sin(Gyro_Deg / 512.0 * 3.14 + 256 - 512) * 20, WHITE);
  display.drawCircle(80, 40, 20, SSD1306_WHITE);
  display.display();

}

  