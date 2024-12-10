#include <Arduino.h>
#include <Servo.h>

#include "motor.hpp"
#include "serial.hpp"

#include "sensors_variables.hpp"

#include "bno055.hpp"
#include "kicker.hpp"
#include "toggle.hpp"

#include "ui.hpp"

#include "attacker.hpp"
#include "defender.hpp"
#include "engelline.hpp"

#define SUPERTEAM_MODE 0

Servo dribbler;

BNO055 bno_imu;
Toggle toggle_start;
Kicker kicker;

Timer mode_change_limit;

bool is_able_to_run = false;

void setup() {
    Serial.begin(9600);

    ui_init();
    serials_init();
    motor_init();
    
    tone(4, 2200, 500);
    delay(600);
    tone(4, 3550, 90);
    delay(110);
    tone(4, 3550, 90);
    delay(110);

    pinMode(22, INPUT_PULLDOWN);

    //bno055 initialize
    bno_imu.init(12);

    kicker.init(2, 3);

    toggle_start.init(5, Toggle::Toggle_Value_Type::PULLDOWN);

    mode_change_limit.start();

    dribbler.attach(30);

    dribbler.writeMicroseconds(2000);
    delay(500);
    dribbler.writeMicroseconds(1000);
    delay(500);

}

void loop() {
    mode_change_limit.tick();

    //Serial reading
    serials_process();

    // gyro processing
    bno_imu.process();
    gyro_deg = bno_imu.get_degrees();

    ui_process();

    line_process();

    if(toggle_start.is_turned() && is_able_to_run/* && analogRead(22) >= 512*/)
    {
        //process
        switch(run_mode)
        {
            case RUN_MODE::ATTACKER_GYRO:
            {
                attacker_process(95);
            }
            break;

            case RUN_MODE::ATTACKER_GYRO_SLOW:
            {
                attacker_process(30);
            }
            break;

            case RUN_MODE::ATTACKER_BLUE:
            {
                attacker_process_cam(cam_blue_deg, cam_blue_dis, cam_yellow_deg, cam_yellow_dis, 95, 0);
            }
            break;

            case RUN_MODE::ATTACKER_BLUE_SLOW:
            {
                attacker_process_cam(cam_blue_deg, cam_blue_dis, cam_yellow_deg, cam_yellow_dis, 95, 1);
            }
            break;

            case RUN_MODE::ATTACKER_YELLOW:
            {
                attacker_process_cam(cam_yellow_deg, cam_yellow_dis, cam_blue_deg, cam_blue_dis, 95, 0);
            }
            break;

            case RUN_MODE::ATTACKER_YELLOW_SLOW:
            {
                attacker_process_cam(cam_yellow_deg, cam_yellow_dis, cam_blue_deg, cam_blue_dis, 95, 1);
            }
            break;

            case RUN_MODE::DEFENDER_GYRO:
            {
                defender_process(false, 0, 0, 0, 0);
            }
            break;

            case RUN_MODE::DEFENDER_YELLOW:
            {
                defender_process(true, cam_yellow_deg, cam_yellow_dis, cam_blue_deg, cam_blue_dis);
            }
            break;

            case RUN_MODE::DEFENDER_BLUE:
            {
                defender_process(true, cam_blue_deg, cam_blue_dis, cam_yellow_deg, cam_yellow_dis);
            }
            break;

            case RUN_MODE::LINE_ONLY:
            {

            }
            break;

            case RUN_MODE::DRIBBLER_TEST:
            {
                dribbler.writeMicroseconds(1300);
            }
            break;
        }
    }
    else
    {
        dribbler.writeMicroseconds(1000);
        motor_direct_drive(0, 0, 0, 0);
    }
}