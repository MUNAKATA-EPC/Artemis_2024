#ifndef _VECTOR_HPP
#define _VECTOR_HPP

#include <Arduino.h>

class Vector{
public:
    Vector(float rad, float size);

    void add(Vector a);
    void sub(Vector a);
    int get_deg();

private:
    float _rad;
    float _size;
};

#endif