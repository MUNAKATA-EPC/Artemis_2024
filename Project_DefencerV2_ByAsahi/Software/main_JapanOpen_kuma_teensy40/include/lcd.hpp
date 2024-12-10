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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
///Button btEnter(30, Button_Type::BUTTON_PULLDOWN);
Button btLeft(31, Button_Type::BUTTON_PULLDOWN);
Button btRight(32, Button_Type::BUTTON_PULLDOWN);
Button btswitch(33, Button_Type::BUTTON_PULLDOWN);


int a_ = 0;
int b_ = 0;
int c_ = 0;



bool running_ = false;
bool aaa = false;

void lcd_setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  //pinMode(30, INPUT_PULLDOWN);

  delay(1000);
}

void lcd_loop()
{
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

  if(a_ == 0)
  {
    display.clearDisplay();
    display.setCursor(30, 10);
    display.println(Ball_Deg);
    display.setCursor(30, 30);
    display.println(Ball_Distance);
    display.setCursor(30, 50);
    display.println(Line_Deg);
    display.setCursor(90, 10);
    display.println(Gyro_Deg);
    display.setCursor(90, 30);
    display.println(running_ == true ? "moving" : "stop" );
    display.display();

  }
  else if(a_ == 1)
  {


    display.clearDisplay();
    display.setCursor(60,30);
    display.setTextSize(1);
    display.setCursor(30, 10);
    display.println(mode);
    display.setCursor(30, 30);
    display.println(line_max);
    display.setCursor(70, 10);
    display.println(Gyro_Deg);
    display.setCursor(70, 30);
    display.println(Yellow_Goal_Deg);
    display.setCursor(70, 50);
    display.println(Yellow_Goal_Distance);
    display.setCursor(30, 50);
    display.println("yellow");
    display.display();

    defence_goal = 0;
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
    display.clearDisplay();
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
    display.println("blue");
    display.display();

    defence_goal = 1;

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
    display.clearDisplay();
    display.setCursor(0,30);
    display.setTextSize(1);
    display.println("attack to yellow");
    display.display();

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
  else if(a_ == 4)
  {
    display.clearDisplay();
    display.setCursor(0,30);
    display.setTextSize(1);
    display.println("attack to blue");
    display.display();

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
    display.clearDisplay();
    display.setCursor(0,30);
    display.setTextSize(1);
    display.println("kick check");
    display.display();

    if(digitalRead(30) == LOW)
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
  }

}


#endif