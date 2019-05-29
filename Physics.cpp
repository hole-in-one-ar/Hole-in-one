//
// Created by zyf on 2019/5/29.
//
#include <iostream>
#include "Physics.h"

void Physics::simulation() {
    //max friction force
    float maxFriction = mass * gravity * mu;
    vector<PhysicsBall> tempBalls(thisBalls);
    vector<PhysicsBall> newBalls;
    vector<Vec3> beforeVector;
    for (int i = 0; i < thisBalls.size(); i++) {
        //if the z coordinate of this ball is smaller than a value, then stop the physical simulation
//            if (thisBalls[i].getOrigin().z > maxZ) {
        //first calculate the velocity of the ball
        //if this is a new added ball, then the velocity is the default one that given by the ball itself
//                Vec3 thisVelo = calVelocity(i, timeStep);
        Vec3 thisVelo = thisBalls[i].getVelocity();
        Vec3 mg = Vec3(0, 0, mass * gravity);
        //transform the velocity to the force
        Vec3 veloForce = Vec3(thisVelo.x * defalutDamping, thisVelo.y * defalutDamping,
                              thisVelo.z * defalutDamping);
        //if the ball is lower than the table(z = 0),
        //the force given by its velocity will have a opposite direction, and lose some energy
        //total force in first step
        Vec3 thisForce = Vec3(mg.x + veloForce.x, mg.y + veloForce.y, mg.z + veloForce.z);
//            }
        //calculate the interaction between each ball
//            if (thisBalls[i].getRadium() - thisBalls[i].getOrigin().z >= 0) {
////                cout << "hit the ground!" << endl;
//                if (!isBallInsideHole(i)) {
////                    cout << "outside hole" << endl;
//                    thisForce = Vec3(thisForce.x * lost, thisForce.y * lost, thisForce.z * (-5) * lost);
//                }
//            }
        Vec3 friction;
        //the friction between the ball and the plane
        //if the result is strange, please delete this part
        if (thisBalls[i].getRadium() - thisBalls[i].getOrigin().z <= thres) {
            friction.x = thisForce.x * mu;
            friction.y = thisForce.y * mu;
            friction.z = thisForce.z * mu;
            if (friction.length() > maxFriction) {
                friction.x = -1 * thisForce.normalize().x * maxFriction;
                friction.y = -1 * thisForce.normalize().y * maxFriction;
                friction.z = -1 * thisForce.normalize().z * maxFriction;
            }
        }
        thisForce.x += friction.x;
        thisForce.y += friction.y;
        thisForce.z += friction.z;
        float deltaMass = timeStep * timeStep / mass;
//            Vec3 moveTrace = Vec3(-thisBalls[i].getOrigin().x + lastBalls[i].getOrigin().x,
//                                  -thisBalls[i].getOrigin().y + lastBalls[i].getOrigin().y,
//                                  -thisBalls[i].getOrigin().z + lastBalls[i].getOrigin().z);
        Vec3 moveTrace = Vec3(thisBalls[i].getOrigin().x - lastBalls[i].getOrigin().x,
                              thisBalls[i].getOrigin().y - lastBalls[i].getOrigin().y,
                              thisBalls[i].getOrigin().z - lastBalls[i].getOrigin().z);

        Vec3 moveVector = Vec3(moveTrace.x + thisForce.x * deltaMass, moveTrace.y + thisForce.y * deltaMass,
                               moveTrace.z + thisForce.z * deltaMass);

        if (thisBalls[i].getRadium() - thisBalls[i].getOrigin().z >= 0) {
//                cout << "hit the ground!" << endl;
            if (!isBallInsideHole(i)) {
//                    cout << "outside hole" << endl;
                moveVector = Vec3(moveVector.x * lost, moveVector.y * lost, -moveVector.z * lost);
            }
        }
        beforeVector.push_back(moveVector);
    }
    for (int i = 0; i < thisBalls.size(); i++) {
        Vec3 moveVector = beforeVector[i];
        if (thisBalls.size() > 1) {
            Vec3 newVector;
            bool isColli = false;
            for (int j = 0; j < thisBalls.size(); j++) {
                if (i != j) {
                    float originDis = distance(thisBalls[i].getOrigin(), thisBalls[j].getOrigin());
                    if (originDis <= thisBalls[i].getRadium() + thisBalls[j].getRadium()) {
//                            cout << "ouch!" << endl;
                        isColli = true;
                        newVector.x += beforeVector[j].x;
                        newVector.y += beforeVector[j].y;
                        newVector.z += beforeVector[j].z;
                    }
                }
            }
            if (isColli) { moveVector = newVector; }
        }
        float thisZ = thisBalls[i].getOrigin().z + moveVector.z;
        Vec3 newOrigin = Vec3(thisBalls[i].getOrigin().x + moveVector.x, thisBalls[i].getOrigin().y + moveVector.y,
                              thisZ);
        Vec3 veloNew = Vec3((-thisBalls[i].getOrigin().x + newOrigin.x) * timeStep,
                            (-thisBalls[i].getOrigin().y + newOrigin.y) * timeStep,
                            (-thisBalls[i].getOrigin().z + newOrigin.z) * timeStep);
        PhysicsBall newBall = PhysicsBall(newOrigin, thisBalls[i].getRadium(), veloNew);
        newBalls.push_back(newBall);
    }
    lastBalls.clear();
    lastBalls = tempBalls;
    thisBalls.clear();
    thisBalls = newBalls;
    //Verlet intergration
}

void Physics::setHole(const float &x, const float &y, const float &r) {
    hole = Hole(x, y, r);
}

void Physics::addBalls(const Vec3 &position, float radium, const Vec3 &velocity) {
    PhysicsBall thisBall = PhysicsBall(position, radium, velocity);
    thisBalls.push_back(thisBall);
    lastBalls.push_back(thisBall);
}

bool Physics::isBallInsideHole(int ballIndex) {
    bool isInside = false;
    auto dis = (float) sqrt((thisBalls[ballIndex].getOrigin().x - hole.getX()) *
                            (thisBalls[ballIndex].getOrigin().x - hole.getX()) +
                            (thisBalls[ballIndex].getOrigin().y - hole.getY()) *
                            (thisBalls[ballIndex].getOrigin().y - hole.getY()));
    if (dis + thisBalls[ballIndex].getRadium() < hole.getRadium()) {
        isInside = true;
    }
    return isInside;
}


float Physics::distance(const Vec3 &a, const Vec3 &b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) +
                (a.z - b.z) * (a.z - b.z));
}

const vector<PhysicsBall> &Physics::getBalls() const {
    return thisBalls;
}

void Physics::setMass(float mass) {
    Physics::mass = mass;
}

void Physics::setDefalutDamping(float defalutDamping) {
    Physics::defalutDamping = defalutDamping;
}

void Physics::setGravity(float gravity) {
    Physics::gravity = gravity;
}

void Physics::setTimeStep(float timeStep) {
    Physics::timeStep = timeStep;
}

void Physics::setMaxZ(float maxZ) {
    Physics::maxZ = maxZ;
}

void Physics::setLost(float lost) {
    Physics::lost = lost;
}
