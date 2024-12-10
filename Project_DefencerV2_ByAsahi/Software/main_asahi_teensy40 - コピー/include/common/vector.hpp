#ifndef _VECTOR_H
#define _VECTOR_H

#include <Arduino.h>

class Vector{
public:
    Vector(float rad, float size)
    {
        _rad = rad;
        _size = size;
    }

    void add(Vector a)
    {
        float origin_vec_x = cos(this->_rad) * this->_size;
        float origin_vec_y = sin(this->_rad) * this->_size;
        float add_vec_x = cos(a._rad) * a._size;
        float add_vec_y = sin(a._rad) * a._size;

        this->_rad = atan2(origin_vec_y + add_vec_y, origin_vec_x + add_vec_x);
        this->_size = sqrt(pow(origin_vec_x + add_vec_x, 2) + pow(origin_vec_y + add_vec_y, 2));
    }

    void sub(Vector a)
    {
        float origin_vec_x = cos(this->_rad) * this->_size;
        float origin_vec_y = sin(this->_rad) * this->_size;
        float add_vec_x = cos(a._rad) * a._size;
        float add_vec_y = sin(a._rad) * a._size;

        this->_rad = atan2(origin_vec_y - add_vec_y, origin_vec_x - add_vec_x);
        this->_size = sqrt(pow(origin_vec_x - add_vec_x, 2) + pow(origin_vec_y - add_vec_y, 2));
    }

    int get_deg()
    {
        return (int)degrees(_rad);
    }
    
private:
    float _rad;
    float _size;
};

#endif