#ifndef _VECTOR_H
#define _VECTOR_H

#include <Arduino.h>

class Vector{
public:
    Vector(float rad, float size);

    void add(Vector a);
    void sub(Vector a);
    int get_deg();
    float get_size();

    Vector get_x_component();
    Vector get_y_component();

private:
    float _rad;
    float _size;
};

#endif