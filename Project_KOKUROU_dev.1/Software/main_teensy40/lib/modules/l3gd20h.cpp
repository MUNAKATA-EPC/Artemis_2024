#include <Arduino.h>
#include <Wire.h>

#include "l3gd20h.hpp"

L3GD20H::L3GD20H(int rtrim_pin)
{
    this->_l3gd20h_rtrim_pin = rtrim_pin;
}

void L3GD20H::read_imu()
{
    short Z = read_data(L3GD20H::L3GD20H_Z_H);
    double z = (Z = (Z << 8) | read_data(L3GD20H_Z_L)) * 100;

    Serial.println(z);

    _l3gd20h_rtrim_caliblation_data = analogRead(_l3gd20h_rtrim_pin);
    _l3gd20h_rtrim_caliblation_data = (_l3gd20h_rtrim_caliblation_data - 512) / 500 * 50;

    _l3gd20h_raw_yaw += z + (_is_l3gd20h_calibration ? 0 : _l3gd20h_auto_caliblation_data);
    _l3gd20h_yaw = _l3gd20h_raw_yaw - _l3gd20h_yaw_offset;
}

void L3GD20H::init()
{
    Wire.begin();

    write_data(L3GD20H_CTRL1, B00001111);
    write_data(L3GD20H_CTRL4, B00110000);
}

void L3GD20H::loop()
{
    if(_is_l3gd20h_calibration)
    {
        if(_l3gd20h_calibration_count == 0)
        {
            _l3gd20h_raw_yaw = 0;
            _l3gd20h_yaw_offset = 0;
            _l3gd20h_auto_caliblation_data = 0;
        }

        read_imu();
        _l3gd20h_calibration_count++;
        
        if(millis() >= _l3gd20h_calibration_start_time + CAL_TIMEMS)
        {
            _l3gd20h_auto_caliblation_data = (0 - _l3gd20h_raw_yaw) / (float)_l3gd20h_calibration_count;
            _l3gd20h_raw_yaw = 0;
            _is_l3gd20h_calibration = false;
        }
    }
    else
    {
        read_imu();
    }
}

void L3GD20H::reset_yaw()
{
    _l3gd20h_yaw_offset = _l3gd20h_raw_yaw;
}

void L3GD20H::calibration_yaw()
{
    _is_l3gd20h_calibration = true;
    _l3gd20h_calibration_count = 0;
    _l3gd20h_calibration_start_time = millis();
}

double L3GD20H::get_auto_calibration_data()
{
    return _l3gd20h_auto_caliblation_data;
}

double L3GD20H::get_rtrim_calibration_data()
{
    return _l3gd20h_rtrim_caliblation_data;
}

double L3GD20H::get_degdata()
{
    double ret = (_l3gd20h_yaw / 480.0 / 100.0 / 155.0 * 90.0);    
    return ret;
}