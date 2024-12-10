#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "lcd.hpp"
#include <Servo.h>
#include <DSR1202.h>
#include "button.hpp"
#include "timer.hpp"
#include "dribbler.hpp"
#include "serial_receive.hpp"
#include "motor.hpp"
#include "main_attack.hpp"
#include "main_defence.hpp"
#include "kicker.hpp"
#include "engelline.hpp"
#include "../lib/vector/vector.hpp"

char near = 'a';
char far = 'b';
char hold = 'c';



void setup(){

    serials_init();
    motor_init();
    lcd_setup();
    
    pinMode(5,INPUT_PULLDOWN) ;
    pinMode(6,INPUT_PULLDOWN)  ; 

 
    //Dribbler.attach(4); 
    //dribbler_setup();
  
}

void loop(){
    
    line_process();
    serials_receive();
    lcd_loop();
    get_line_deg__();


    Serial.print(digitalRead(11));
    Serial.print(" ");
    Serial.println();
    

    
}


//aがまもり
//ｂがあｔっく
