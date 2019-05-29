#include <iostream>
#include "PhysicsBall.h"
#include "Hole.h"
#include "Physics.h"
#include "Vec3.h"

using namespace std;

int main() {
    //initialization
    Physics py = Physics();
    //set the hole value, (0, 0) is the origin of the hole, 3 is the radium of the hole.
    py.setHole(0, 0, 3);
    //add balls
    py.addBalls(Vec3(0, -5, 12), 2, Vec3(0, 200, 0));
    py.addBalls(Vec3(0, 5, 12), 2, Vec3(0, -200, 0));
    //testing data
    int a = 500;
    while (a--) {
        //the time step is 1/60
        //recommend 30 times of physical simulation per 1 frame
        for (int i = 0; i < 30; i++) {
            py.simulation();
        }
        cout << "1st Ball: "<< py.getBalls()[0].getOrigin().x << ", " << py.getBalls()[0].getOrigin().y << ", "
             << py.getBalls()[0].getOrigin().z;
        cout << ",\t2nd Ball: "<< py.getBalls()[1].getOrigin().x << ", " << py.getBalls()[1].getOrigin().y << ", "
             << py.getBalls()[1].getOrigin().z << endl;
    }
    return 0;
}