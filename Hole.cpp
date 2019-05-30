#include "Hole.h"

Hole::Hole() {
    x = 0;
    y = 0;
    radium = 1;
}

Hole::Hole(float xIn, float yIn, float rIn) {
    x = xIn;
    y = yIn;
    radium = rIn;
}


Hole::Hole(const Hole &hole) {
    x = hole.x;
    y = hole.y;
    radium = hole.radium;
}

float Hole::getX() const {
    return x;
}

void Hole::setX(float x) {
    Hole::x = x;
}

float Hole::getY() const {
    return y;
}

void Hole::setY(float y) {
    Hole::y = y;
}

float Hole::getRadium() const {
    return radium;
}

void Hole::setRadium(float radium) {
    Hole::radium = radium;
}
