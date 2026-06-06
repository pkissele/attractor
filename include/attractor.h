#pragma once
#include <atomic>
#include <sstream>
#include <vector>
#include <unordered_set>

#include "points.h"

#include <Eigen/Dense>
using Vec3 = Eigen::Vector3f;

class Attractor {
    public:
        Attractor(int N, double viewW, double viewH);

        bool step(double dt, int curStep);

        Points& getPoints() { return points; }

        const int getN() const { return N; }

        double viewW, viewH;

        int N;

    private:
        int maxN;

        Points points;

        Vec3 curLoc;

        std::ostringstream buffer;
};
