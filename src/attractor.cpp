#include "attractor.h"

#include <cassert>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>

#include <Eigen/Dense>

#include "points.h"
#include "timer.h"

#include "consts.h"


using namespace Consts;
using namespace std;

using Vec3 = Eigen::Vector3f;


// Lorenz Attractor Equations
inline float dxdt(float x, float y, float z) {
    return SIGMA * (y - x);
}
inline float dydt(float x, float y, float z) {
    return x * (RHO - z) - y;
}
inline float dzdt(float x, float y, float z) {
    return x * y - BETA * z;
}


bool Attractor::step(double dt, int curStep) {
    // Using Runge Kutta

    float k1x = dxdt(curLoc[0], curLoc[1], curLoc[2]);
    float k1y = dydt(curLoc[0], curLoc[1], curLoc[2]);
    float k1z = dzdt(curLoc[0], curLoc[1], curLoc[2]);

    float k2x = dxdt(curLoc[0] + dt * k1x/2, curLoc[1] + dt * k1y/2, curLoc[2] + dt * k1z/2);
    float k2y = dydt(curLoc[0] + dt * k1x/2, curLoc[1] + dt * k1y/2, curLoc[2] + dt * k1z/2);
    float k2z = dzdt(curLoc[0] + dt * k1x/2, curLoc[1] + dt * k1y/2, curLoc[2] + dt * k1z/2);

    float k3x = dxdt(curLoc[0] + dt * k2x/2, curLoc[1] + dt * k2y/2, curLoc[2] + dt * k2z/2);
    float k3y = dydt(curLoc[0] + dt * k2x/2, curLoc[1] + dt * k2y/2, curLoc[2] + dt * k2z/2);
    float k3z = dzdt(curLoc[0] + dt * k2x/2, curLoc[1] + dt * k2y/2, curLoc[2] + dt * k2z/2);

    float k4x = dxdt(curLoc[0] + dt * k3x, curLoc[1] + dt * k3y, curLoc[2] + dt * k3z);
    float k4y = dydt(curLoc[0] + dt * k3x, curLoc[1] + dt * k3y, curLoc[2] + dt * k3z);
    float k4z = dzdt(curLoc[0] + dt * k3x, curLoc[1] + dt * k3y, curLoc[2] + dt * k3z);


    curLoc[0] += dt/6 * (k1x + 2 * k2x + 2 * k3x + k4x);
    curLoc[1] += dt/6 * (k1y + 2 * k2y + 2 * k3y + k4y);
    curLoc[2] += dt/6 * (k1z + 2 * k2z + 2 * k3z + k4z);

    points.px[curStep%N] = curLoc[0];
    points.py[curStep%N] = curLoc[1];
    points.pz[curStep%N] = curLoc[2];
    points.curInd = curStep%N;

    return false;
}


Attractor::Attractor(int N_, double viewW_, double viewH_) : maxN(N_), viewW(viewW_), viewH(viewH_), points(N_) {
    N = maxN;
    curLoc[0] = 1;
    curLoc[1] = 1;
    curLoc[2] = 1;

    for (int i = 0; i < N; i++) {
        points.px[0] = 0.0f;
        points.py[0] = 0.0f;
        points.pz[0] = 0.0f;
    }
}
