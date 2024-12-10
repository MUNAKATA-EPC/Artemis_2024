#ifndef _SENSORS_VARIABLES_HPP
#define _SENSORS_VARIABLES_HPP

#include "timer.hpp"

#define USING_WIRELESS 0

// ball sensor
extern int ball_deg;
extern int ball_distance;

// line sensor
extern int line_value;

// [0] ~ [15]   : engel line
// [16]         : right
// [17]         : back
// [18]         : left
extern int line_data[19];

// camera
extern int cam_yellow_deg;
extern int cam_yellow_dis;
extern int cam_blue_deg;
extern int cam_blue_dis;

extern int gyro_deg;

extern int hold_value;

extern int ally_mode;

enum RUN_MODE
{
    ATTACKER_GYRO,
    ATTACKER_GYRO_SLOW,
    ATTACKER_YELLOW,
    ATTACKER_YELLOW_SLOW,
    ATTACKER_BLUE,
    ATTACKER_BLUE_SLOW,
    DEFENDER_GYRO,
    DEFENDER_YELLOW,
    DEFENDER_BLUE,
    DRIBBLER_TEST,
    LINE_ONLY,
};

extern enum RUN_MODE run_mode;

extern Timer mode_change_limit;

#endif