#ifndef _LCD_HPP
#define _LCD_HPP

#include <Arduino.h>

#include "button.hpp"
#include "serial_receive.hpp"
#include "dribbler.hpp"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "motor.hpp"
#include "main_defence.hpp"
#include "main_attack.hpp"
#include "main_attack_copy.hpp"
#include "timer.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
///Button btEnter(30, Button_Type::BUTTON_PULLDOWN);
Button btLeft(31, Button_Type::BUTTON_PULLDOWN);
Button btRight(32, Button_Type::BUTTON_PULLDOWN);
Button btswitch(33, Button_Type::BUTTON_PULLDOWN);

Timer send_timer;
int send_check_time = 0;

int a_ = 0;
int b_ = 0;
int c_ = 0;



bool enter = false;
bool running_ = false;
bool aaa = false;

void lcd_setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  send_timer.start();
  pinMode(30, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
  pinMode(6, INPUT_PULLDOWN);

  delay(1000);
}

void lcd_loop()
{
  send_timer.tick();

  //btEnter.loop();
  btLeft.loop();
  btRight.loop();
  kicker.tick();

  if(btswitch.isPushed())
  {
    running_ = !running_;
  }

  if(btRight.isPushed())
  {
    if(a_ + 1 <= 5)
    {
      a_++;
    }
  }

  if(btLeft.isPushed())
  {
    if(a_ - 1 >= 0)
    {
      a_--;
    }
  }

  
  display.clearDisplay();

  if(a_ == 0)
  {
    display.setCursor(30, 10);
    display.println(Ball_Deg);
    display.setCursor(30, 30);
    display.println(Ball_Distance);
    display.setCursor(30, 50);
    display.println(line_deg_);
    display.setCursor(90, 10);
    display.println(Gyro_Deg);
    display.setCursor(90, 30);
    display.println(running_ == true ? "moving" : "stop" );

  }
  else if(a_ == 1)
  {
    display.setCursor(60,30);
    display.setTextSize(1);
    display.setCursor(30, 10);
    display.println(digitalRead(11));
    display.setCursor(30, 30);
    display.println(digitalRead(11));
    display.setCursor(70, 10);
    display.println(Gyro_Deg);
    display.setCursor(70, 30);
    display.println(Yellow_Goal_Deg);
    display.setCursor(70, 50);
    display.println(Yellow_Goal_Distance);
    display.setCursor(30, 50);
    display.println("yellow");

    attack_goal = 0;
    if(digitalRead(33) == 0)
    {   
      motor_power(0, 0, 0, 0);
      Dribbler.writeMicroseconds(1000);
    }
    else
    {
      attack.main();
    }
  }
  else if(a_ == 2)
  {
    display.setCursor(60,30);
    display.setTextSize(2);
    display.setCursor(30, 10);
    display.println(line_min);
    display.setCursor(30, 30);
    display.println(line_max);
    display.setCursor(70, 10);
    display.println(Gyro_Deg);
    display.setCursor(70, 30);
    display.println(Blue_Goal_Deg);
    display.setCursor(70, 50);
    display.println(Blue_Goal_Distance);
    display.setCursor(30, 50);
    display.println("defence");

    defence_goal = 0;

    if(digitalRead(33) == 0)
    {   
      motor_power(0, 0, 0, 0);
      Dribbler.writeMicroseconds(1000);
    }
    else
    {
      defence.main();        
    }
  }
  else if(a_ == 3)
  {
    display.setCursor(30, 10);
    display.println(Court_Deg * 2);
    display.setCursor(0,30);
    display.setTextSize(1);
    display.println("attack gyro");

    attack_goal = 0;
    if(digitalRead(33) == 0)
    {   
      motor_power(0, 0, 0, 0);
      Dribbler.writeMicroseconds(1000);
    }
    else
    {

               
      attack_gyro.main();  

    
    }
  }
  else if(a_ == 4)
  {
    display.setCursor(0,30);
    display.setTextSize(1);
    display.println("attack to blue");
    attack_goal = 1;
    if(digitalRead(33) == 0)
    {   
      motor_power(0, 0, 0, 0);
      Dribbler.writeMicroseconds(1000);
    }
    else
    {
      attack.main();

   
    }
  }
  else if(a_ == 5)
  {
    display.setCursor(0,30);
    display.setTextSize(1);
    display.println("kick check");

    if(digitalRead(30) == 1)
    {   
      
      analogWrite(2, 0);
      analogWrite(3, 1023);
      motor_power(0, 0, 0, 0);
    }
    else
    {

      analogWrite(2, 1023);
      analogWrite(3, 0);  
    }

    
    if(digitalRead(33) == 0)
    {   
      motor_power(0, 0, 0, 0);
      Dribbler.writeMicroseconds(1000);
    }
    else
    {
      Dribbler.writeMicroseconds(1060);
    }
  }

  if(send_timer.get_value() >= send_check_time)
  {
    send_check_time = send_timer.get_value() + 100;
    display.display();
  }

}


#endif