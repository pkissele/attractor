#include "draw.h"

#include <Eigen/Dense>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_utils.h"

using namespace std;


using Mat4f = Eigen::Matrix4f;
using Vec3f = Eigen::Vector3f;



void drawPoint(GLuint program, GLuint vao, GLuint vbo, Vec3f pos, Vec3f color, float size, Mat4f mvp) {
    float data[] = {pos[0], pos[1], pos[2]};
    uploadBuffer(vbo, data, sizeof(data));

    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"),      1, GL_FALSE, mvp.data());
    glUniform3f(      glGetUniformLocation(program, "color"),     color.x(), color.y(), color.z());
    glUniform1f(      glGetUniformLocation(program, "pointSize"), size);
    drawVAO(program, vao, GL_POINTS, 1);
}
