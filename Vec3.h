
#include <iostream>
#include <cmath>

using namespace std;

#ifndef __VEC3__H__
#define __VEC3__H__
class Vec3 {
public:
    float x;
    float y;
    float z;

    Vec3();

    Vec3(float xIn, float yIn, float zIn);

    Vec3 operator+(const Vec3 &b);

    Vec3 operator-(const Vec3 &b);

    float operator*(const Vec3 &b);

    Vec3 operator*(float b);

    Vec3 normalize();

    float length();

    Vec3(const Vec3 &v);
};

#endif