#ifndef _BALL_TRACKING_TEST_H
#define _BALL_TRACKING_TEST_H

#include <Arduino.h>

#include "common/timer.hpp"
#include "common/motor.hpp"
#include "common/vector.hpp"
#include "common/serial_receive.hpp"

#include "running/normal_running_gyropid.hpp"
#include "running/normal_running_goalpid.hpp"

#include "common/processing/ball_tracking.hpp"

class Ball_Tracking_Test{
    void main_running_with_gyro()
    {
        ball_tracking_gyro.ball_tracking_main(normal_running_gyropid.get_robo_speed());

        move(ball_tracking_gyro.get_degrees(), ball_tracking_gyro.get_speed());
    }

    void main_running_with_camera(int goal_type)
    {
        int camera_data[2] = {0, 0};
        camera_data[0] = goal_type == 0 ? Yellow_Goal_Deg : Blue_Goal_Deg;
        camera_data[1] = goal_type == 0 ? Yellow_Goal_Distance : Blue_Goal_Distance;

        ball_tracking_camera.ball_tracking_main(normal_running_goalpid.get_robo_speed(), camera_data);

        move(ball_tracking_camera.get_degrees(), ball_tracking_camera.get_speed());
    }
};

#endif