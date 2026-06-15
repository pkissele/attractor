#include <Eigen/Dense>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_utils.h"

using Mat4f = Eigen::Matrix4f;
using Vec3f = Eigen::Vector3f;



void drawPoint(GLuint program, GLuint vao, GLuint vbo, Vec3f pos, Vec3f color, float size, Mat4f mvp);
