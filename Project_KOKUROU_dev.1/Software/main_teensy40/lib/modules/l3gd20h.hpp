#include <Arduino.h>
#include <Wire.h>

#ifndef _L3GD20H_HPP
#define _L3GD20H_HPP

class L3GD20H{

private:
    const byte L3GD20H_ADDR = B1101010;  // SA0 = GND
    //const byte L3GD20_ADDR = B1101011;// SA0 = VDD_IO

    const byte L3GD20H_WHOAMI = 0x0F;
    const byte L3GD20H_CTRL1 = 0x20;
    const byte L3GD20H_CTRL2 = 0x21;
    const byte L3GD20H_CTRL3 = 0x22;
    const byte L3GD20H_CTRL4 = 0x23;
    const byte L3GD20H_CTRL5 = 0x24;
    const byte L3GD20H_X_L = 0x28;
    const byte L3GD20H_X_H = 0x29;
    const byte L3GD20H_Y_L = 0x2A;
    const byte L3GD20H_Y_H = 0x2B;
    const byte L3GD20H_Z_L = 0x2C;
    const byte L3GD20H_Z_H = 0x2D;

    const int CAL_TIMEMS = 3000;

    int _l3gd20h_rtrim_pin;

    double _l3gd20h_yaw;
    double _l3gd20h_raw_yaw;
    double _l3gd20h_yaw_offset;
    
    double _l3gd20h_auto_caliblation_data;
    double _l3gd20h_rtrim_caliblation_data;

    bool _is_l3gd20h_calibration;
    int _l3gd20h_calibration_count;
    int _l3gd20h_calibration_start_time;

    void read_imu();
    
    void write_data(byte reg, byte val)
    {
        Wire.beginTransmission(L3GD20H_ADDR);
        Wire.write(reg);
        Wire.write(val);
        Wire.endTransmission();  
    }

    byte read_data(byte reg)
    {
        byte ret = 0;
        Wire.beginTransmission(L3GD20H_ADDR);
        Wire.write(reg);
        Wire.endTransmission();
        Wire.requestFrom((unsigned int)L3GD20H_ADDR, 1);
        
        while (Wire.available()) {
            ret = Wire.read();
        }
        
        return ret;
    }

public:
    L3GD20H(int rtrim_pin);

    void init();
    void loop();

    void reset_yaw();
    void calibration_yaw();

    double get_auto_calibration_data();
    double get_rtrim_calibration_data();
    double get_degdata();
};

#endif