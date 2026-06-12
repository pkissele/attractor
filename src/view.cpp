#include "view.h"

#include <vector>

#include <Eigen/Dense>

using namespace std;

using Mat4f = Eigen::Matrix4f;
using Vec3f = Eigen::Vector3f;


Mat4f makePerspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / tan(fov * 0.5f);
    Mat4f m = Mat4f::Zero();
    m(0, 0) = f / aspect;
    m(1, 1) = f;
    m(2, 2) = (far + near) / (near - far);
    m(2, 3) = (2 * far * near) / (near - far);
    m(3, 2) = -1.0f;
    return m;
}

Mat4f makeOrtho(float width, float height, float near, float far) {
    Mat4f m = Mat4f::Zero();
    m(0, 0) = 2.0f / width;
    m(1, 1) = 2.0f / height;
    m(2, 2) = 2.0f / (near - far);
    m(2, 3) = (far + near) / (near - far);
    m(3, 3) = 1.0f;
    return m;
}

Mat4f makeView(Vec3f eye, Vec3f target, Vec3f up) {
    Vec3f f = (target - eye).normalized();
    Vec3f r = f.cross(up).normalized();
    Vec3f u = r.cross(f);
    Mat4f m = Mat4f::Identity();
    m(0, 0) =  r.x(); m(0, 1) =  r.y(); m(0, 2) =  r.z(); m(0, 3) = -r.dot(eye);
    m(1, 0) =  u.x(); m(1, 1) =  u.y(); m(1, 2) =  u.z(); m(1, 3) = -u.dot(eye);
    m(2, 0) = -f.x(); m(2, 1) = -f.y(); m(2, 2) = -f.z(); m(2, 3) =  f.dot(eye);
    return m;
}
