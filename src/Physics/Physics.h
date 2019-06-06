#pragma once
#include <iostream>
#include "PhysicsBall.h"
#include "Vec3.h"
#include "Hole.h"
#include <vector>
#include <cmath>

using namespace std;
#ifndef __PHYSICS__H__
#define __PHYSICS__H__

class Physics {
private:
    //hole on the plane
    Hole hole;
    //vector to store current balls position and velocity
    vector<PhysicsBall> thisBalls;
    //vector to store last balls position and velocity, used for Verlet integration
    vector<PhysicsBall> lastBalls;
    //mass of the ball, default value is 1

private:
    //an imaginary force
    float defalutDamping = 0.03f;
    //gravity force
    float gravity = -0.085;
    //simulation time steps
    float timeStep = 1.0f / 60;
    //simulation stop value
    float maxZ = -10;
    //the enegry lost after collision
    float lost = 0.95f;
    //max move amount
    float moveAmount = 0.5f;
    //the min distance between ball and plane
    float thres = 0.00001f;
    //friction parameter
    float mu = -0.5f;

    /*************************************************
    Function:       isBallInsideHole
    Description:    judge the ball is inside the hole or not, for plane collision detection
    Input:          int ballIndex: the index of the ball in vector thisballs
    Output:         true: the ball is inside the hole
                    false: the ball is outside the hole
    Hint:           the precision maybe not so good, will have some error
    *************************************************/
    bool isBallInsideHole(int ballIndex);

    /*************************************************
    Function:       distance
    Description:    calculate the distance between two Vec3 values
    Input:          Vec3 a: the first position
                    Vec3 b: the second position
    Output:         float value, the distance between a and b
    Hint:           this function is best in Vec3 class, but I forgot it when I made it
    *************************************************/
    static float distance(const Vec3 &a, const Vec3 &b);

public:
    /*************************************************
    Function:       setHole
    Description:    set the information of the hole one the plane
    Input:          float x: x coordinate of the hole
                    float y: y coordinate of the hole
                    float r: radium of the hole
    *************************************************/
    void setHole(const float &x, const float &y, const float &r);

    /*************************************************
    Function:       addBalls
    Description:    adding balls for physical simulation
    Input:          Vec3 position: the position of the ball
                    float radium: the radium of the ball
                    Vec3 velocity: the initial velocity of the ball
                    float mass: the mass of the ball
    *************************************************/
    void addBalls(const Vec3 &position, float radium, const Vec3 &velocity, const float &mass);

    /*************************************************
    Function:       setDefalutDamping
    Description:    set the parameter to calculate force from velocity of a ball
    Input:          float defalutDamping: the value of the new parameter
    Others:         the default value is 0.03
    *************************************************/
    void setDefalutDamping(float defalutDamping);

    /*************************************************
    Function:       setGravity
    Description:    set the gravity
    Input:          float gravity: the value of the new gravity
    Others:         the default value is -0.085
                    make sure to set a negative number
    *************************************************/
    void setGravity(float gravity);

    /*************************************************
    Function:       setTimeStep
    Description:    set the time step for Verlet integration
    Input:          float timeStep: the value of the new time step
    Others:         the default value is 1/60
                    if you want to make the simulation slower, set the value to 1/120
                    if you want to make the simulation faster, set the value to 1/30
                    don't set too big value, it will make the simulation collapse
    *************************************************/
    void setTimeStep(float timeStep);

    /*************************************************
    Function:       setMaxZ
    Description:    set the max value of z coordinate in negative planar
    Input:          float maxZ: the value of the new max value of z coordinate
    Others:         the default value is -10
                    if the ball goes lower than this value, the physical simulation will stop
    *************************************************/
    void setMaxZ(float maxZ);

    /*************************************************
    Function:       setLost
    Description:    set the cost of each collision with the plane ground
    Input:          float lost: the value of the new lost value
    Others:         the default value is 0.8
                    it must lower than 1 and bigger than 0
    *************************************************/
    void setLost(float lost);

    /*************************************************
    Function:       simulation
    Description:    doing the physical simulation
    Others:         the default time step is 1/60, recommend to do 30 times
                    physical simulation per 1 frame
    *************************************************/
    void simulation();

    /*************************************************
    Function:       getBalls
    Description:    get the vector of all the balls
    Others:         use it to render the balls
    *************************************************/
    const vector<PhysicsBall> &getBalls() const;

    void removeBall(int num);
};


#endif