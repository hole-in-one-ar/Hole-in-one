#include "PhysicsBall.h"

PhysicsBall::PhysicsBall() {
    origin = Vec3();
    radium = 1;
    velocity = Vec3();

}

PhysicsBall::PhysicsBall(const Vec3 &originIn, const float &rIn, const Vec3 &velo, const float &massIn) {
    origin = originIn;
    radium = rIn;
    velocity = velo;
    mass = massIn;
}

PhysicsBall::PhysicsBall(const float &oInx, const float &oIny, const float &oInz, const float &rIn, const float &vInx,
                         const float &vIny, const float &vInz, const float& massIn) {
    origin = Vec3(oInx, oIny, oInz);
    radium = rIn;
    velocity = Vec3(vInx, vIny, vInz);
}

PhysicsBall::PhysicsBall(const PhysicsBall &ball) {
    origin = ball.origin;
    radium = ball.radium;
    velocity = ball.velocity;
    mass = ball.mass;
}

const Vec3 &PhysicsBall::getOrigin() const {
    return origin;
}

void PhysicsBall::setOrigin(const Vec3 &origin) {
    PhysicsBall::origin = origin;
}

float PhysicsBall::getRadium() const {
    return radium;
}

void PhysicsBall::setRadium(float radium) {
    PhysicsBall::radium = radium;
}

const Vec3 &PhysicsBall::getVelocity() const {
    return velocity;
}

void PhysicsBall::setVelocity(const Vec3 &velocity) {
    PhysicsBall::velocity = velocity;
}

float PhysicsBall::getMass() const {
    return mass;
}

void PhysicsBall::setMass(float mass) {
    PhysicsBall::mass = mass;
}
