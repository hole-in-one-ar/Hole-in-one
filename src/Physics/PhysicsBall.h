#pragma once

#include <iostream>
#include "Vec3.h"

using namespace std;
#ifndef __PHYSICS__BALL__H__
#define __PHYSICS__BALL__H__

class PhysicsBall {
private:
    Vec3 origin;
    float radium;
    Vec3 velocity;
    float mass;

public:
    PhysicsBall();

    PhysicsBall(const Vec3 &originIn, const float &rIn, const Vec3 &velo, const float& massIn);

    PhysicsBall(const float &oInx, const float &oIny, const float &oInz, const float &rIn, const float &vInx,
                const float &vIny, const float &vInz, const float& massIn);

    PhysicsBall(const PhysicsBall &ball);

    const Vec3 &getOrigin() const;

    void setOrigin(const Vec3 &origin);

    float getRadium() const;

    void setRadium(float radium);

    const Vec3 &getVelocity() const;

    void setVelocity(const Vec3 &velocity);

    float getMass() const;

    void setMass(float mass);
};

#endif