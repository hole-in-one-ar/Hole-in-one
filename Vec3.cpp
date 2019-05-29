#include "Vec3.h"

Vec3::Vec3() {
    x = 0;
    y = 0;
    z = 0;
}

Vec3::Vec3(float xIn, float yIn, float zIn) {
    x = xIn;
    y = yIn;
    z = zIn;
}

Vec3 Vec3::operator+(const Vec3 &b) {
    Vec3 a;
    a.x = this->x + b.x;
    a.y = this->y + b.y;
    a.z = this->z + b.z;
    return a;
}

float Vec3::operator*(const Vec3 &b) {
    return this->x * b.x + this->y * b.y + this->z * b.z;
}

Vec3 Vec3::operator-(Vec3 &b) {
    Vec3 a;
    a.x = this->x - b.x;
    a.y = this->y - b.y;
    a.z = this->z - b.z;
    return a;
}

Vec3 Vec3::operator*(float b) {
    Vec3 a;
    a.x = this->x * b;
    a.y = this->y * b;
    a.z = this->z * b;
    return a;
}

Vec3 Vec3::normalize() {
    Vec3 a;
    float length = this->length();
    a.x = this->x / length;
    a.y = this->y / length;
    a.z = this->z / length;
    return a;
}

float Vec3::length() {
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

Vec3::Vec3(const Vec3 &v) {
    x = v.x;
    y = v.y;
    z = v.z;
}