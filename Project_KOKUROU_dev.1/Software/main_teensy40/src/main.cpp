#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Servo.h>

#include "sensors_variables.hpp"
#include "serial_receive.hpp"

#include "line.hpp"
#include "line_test.hpp"
#include "attacker.hpp"
#include "defender.hpp"
#include "neopixel.hpp"

#include "l3gd20h.hpp"
#include "vector.hpp"
#include "button.hpp"
#include "toggle.hpp"
#include "kicker.hpp"
#include "motor.hpp"

#include "ui.hpp"

L3GD20H l3gd20h(27);

Button btEnter;
Button btLeft;
Button btRight;
Button btGyroReset;

Toggle tgRun;

Kicker kicker;

Servo dribbler;

bool is_able_to_run;
int type_of_run;

extern bool is_dribbler_roll;

void setup() {
    Serial.begin(9600);

    tone(6, 2000, 500);
    delay(600);
    tone(6, 3500, 90);
    delay(110);
    tone(6, 3500, 90);
    delay(110);

    serials_init();
    motor_init();
    neopixel_init();
    ui_init();

    l3gd20h.init();
    l3gd20h.calibration_yaw();

    btEnter.init(11, Button::PULLDOWN);
    btLeft.init(10, Button::PULLDOWN);
    btRight.init(12, Button::PULLDOWN);
    btGyroReset.init(23, Button::PULLDOWN);

    tgRun.init(9, Toggle::PULLDOWN);
    
    kicker.init(3, 4);
    
    dribbler.attach(2);
    dribbler.writeMicroseconds(2000);
    delay(500);
    dribbler.writeMicroseconds(1000);
    delay(500);
}

float iaa = 0;

void loop() {
    btEnter.loop();
    btLeft.loop();
    btRight.loop();
    btGyroReset.loop();

    kicker.loop();

    serials_read();

    line_loop();

    //ソースコードの記述にあたってフォントの変更を行うのは非常に気持ちのいいことである。
    //neopixel_indicator_loop();
    //neopixel_engel_loop();

    if(tgRun.is_turned() && is_able_to_run)
    {
        if(type_of_run == 0)        //gyro
        {
            attacker_gyro_loop();
        }
        else if(type_of_run == 1)   //yellow
        {
            attacker_cam_loop(0);
        }
        else if(type_of_run == 2)   //blue
        {
            attacker_cam_loop(1);
        }
        else if(type_of_run == 3)
        {
            line_test_loop();
        }
    }
    else
    { 
        motor_direct_drive(0, 0, 0);

        if(!is_dribbler_roll)
        {
            dribbler.writeMicroseconds(1000);
        }
    }

    ui_loop();
}