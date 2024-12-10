#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "button.h"
#include "sensors.h"
#include "DSR1202.h"

#define PIN_ENTER 30
#define PIN_LEFT  31
#define PIN_RIGHT 32

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

DSR1202 motordriver(0);

Button LCD_ENTER(PIN_ENTER);
Button LCD_RIGHT(PIN_RIGHT);
Button LCD_LEFT(PIN_LEFT);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool bGameMode;

int Line_deg = 0;

void setup() {
  Serial.begin(9600);

  Serial6.begin(115200);
  Serial6.setTimeout(5);
  Serial7.begin(19200);
  Serial7.setTimeout(5);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);
  Initialize_Sensors();
 

  pinMode(30, INPUT_PULLDOWN);
  pinMode(31, INPUT_PULLDOWN);
  pinMode(32, INPUT_PULLDOWN);

  motordriver.Init();
}

int i = 0;

void loop() {
  String val;
  if(Serial7.available())
  {
    val = Serial7.readString();
  }
  //a = val[1];
 // Serial.println(val[0]);
  char line[8] = {val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7]};
  if(line[0] == '0')
  {
    Line_deg = 45;
  }
  else if(line[1] == '0')
  {
    Line_deg = 90;
  }
  else
  {
    Line_deg = 0;
  }


  Serial.println(Line_deg);

  IR_sensors();
  display.clearDisplay();
  if(bGameMode)
  {
    display.setTextSize(1);     //文字サイズ（1）
    display.setCursor(4, 0);    //表示開始位置左上角（X,Y）
    display.println("RUNNING!!");    //表示内容
  }
  else
  {
    display.setTextSize(1);     //文字サイズ（1）
    display.setCursor(4, 0);    //表示開始位置左上角（X,Y）
    display.println("PUSH ENTER BUTTON");    //表示内容
  }

int IR_dis = 1100;
int IR_deg = 10;

  for(int i = 0; i < 8; i++){
    if(IR_sen[i] < IR_dis){
      IR_dis = IR_sen[i];
      IR_deg = i * 360 / 8;
    }
  }

  
  LCD_ENTER.loop();
  LCD_RIGHT.loop();
  LCD_LEFT.loop();

  if(LCD_RIGHT.isPushed())
  {
    i = 0;
  }
  if(LCD_LEFT.isPushed())
  {
    i--;
  }
  if(LCD_ENTER.isPushed())
  {
    bGameMode = !bGameMode;
  }

  float gyro_val = Serial6.readString().toFloat();

  display.setCursor(0, 50);   //表示開始位置左上角（X,Y）
  //display.println(gyro_val);    //表示内容

  display.setCursor(68, 10);
  display.println(val);
  display.drawLine(80, 40, 80 + cos(gyro_val / 512.0 * 3.14 + 256) * 20, 40 + sin(gyro_val / 512.0 * 3.14 + 256) * 20, WHITE);
  display.drawCircle(80, 40, 20, SSD1306_WHITE);
  display.display();

  

  if(bGameMode)
  {
    int deg = IR_deg;
    int power = 70;
    double Power[] = {-sin((deg - 30) / 180.0 * 3.14) * power,
    sin((deg + 30) / 180.0 * 3.14) * power,
    -sin((deg - 135) / 180.0 * 3.14) * power,
    sin((deg + 135) / 180.0 * 3.14) * power};
    motordriver.move(Power[0], Power[1], Power[2], Power[3], false);
  }
  else
  {
    motordriver.move(0, 0, 0, 0, true);
  }
}
