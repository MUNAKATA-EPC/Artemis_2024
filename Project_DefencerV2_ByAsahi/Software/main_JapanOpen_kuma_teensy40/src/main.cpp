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
#include "../lib/vector/vector.hpp"




void setup(){

    serials_init();
    motor_init();
    lcd_setup();
    Dribbler.attach(4);
    dribbler_setup();
    
}

void loop(){

    serials_receive();
    lcd_loop();


}





/*            if(temp - 10 < Ball_Deg && Ball_Deg < temp + 10)
            {
                move_deg = 0;
                move_speed = 0;
            }
            else if(temp_ - 10 < Ball_Deg && Ball_Deg < temp_ + 10)
            {
                move_deg = 0;
                move_speed = 0;
            }
            else if(temp < Ball_Deg)
            {
                move_deg = line_right;
                move_speed = 30;
            }
            else if(temp > Ball_Deg)
            {
                move_deg = line_left;
                move_speed = 30;
            }*/