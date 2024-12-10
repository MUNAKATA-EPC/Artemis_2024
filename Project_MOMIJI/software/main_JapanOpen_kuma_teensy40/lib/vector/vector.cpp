#include <Arduino.h>
#include "vector.hpp"

Vector::Vector(float rad, float size)
{
    this->_rad = rad;
    this->_size = size;
}

void Vector::add(Vector a)
{
    float origin_vec_x = cos(this->_rad) * this->_size;
    float origin_vec_y = sin(this->_rad) * this->_size;
    float add_vec_x = cos(a._rad) * a._size;
    float add_vec_y = sin(a._rad) * a._size;

    this->_rad = atan2(origin_vec_y + add_vec_y, origin_vec_x + add_vec_x);
    this->_size = sqrt(pow(origin_vec_x + add_vec_x, 2) + pow(origin_vec_y + add_vec_y, 2));
}

void Vector::sub(Vector a)
{
    float origin_vec_x = cos(this->_rad) * this->_size;
    float origin_vec_y = sin(this->_rad) * this->_size;
    float add_vec_x = cos(a._rad) * a._size;
    float add_vec_y = sin(a._rad) * a._size;

    this->_rad = atan2(origin_vec_y - add_vec_y, origin_vec_x - add_vec_x);
    this->_size = sqrt(pow(origin_vec_x - add_vec_x, 2) + pow(origin_vec_y - add_vec_y, 2));
}

int Vector::get_deg()
{
    return (int)degrees(_rad);
}