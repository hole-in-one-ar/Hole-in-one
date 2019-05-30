#pragma once

#include <iostream>

using namespace std;

#ifndef __HOLE__H__
#define __HOLE__H__

class Hole {
private:
    float x;
    float y;
    float radium;

public:
    Hole();

    Hole(float xIn, float yIn, float rIn);

    Hole(const Hole &hole);

    float getX() const;

    void setX(float x);

    float getY() const;

    void setY(float y);

    float getRadium() const;

    void setRadium(float radium);
};

#endif
