#pragma once

#include <vector>

#include <Eigen/Dense>

using namespace std;

using Mat4f = Eigen::Matrix4f;
using Vec3f = Eigen::Vector3f;


Mat4f makePerspective(float fov, float aspect, float near, float far);

Mat4f makeOrtho(float width, float height, float near, float far);

Mat4f makeView(Vec3f eye, Vec3f target, Vec3f up);
