#pragma once
#include <string>
#include <vector>

#include "points.h"
#include "attractor.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "atomic"
#include "mutex"

struct Buffer {
    int N, curInd;
    std::vector<float> px, py, pz, age;

    Buffer() = default;
    Buffer(int n) : N(n) {
        px.resize(n);
        py.resize(n);
        pz.resize(n);
        age.resize(n);
    }
};

struct globalState {
    // Keybinds
    bool& save_frames;
    bool& save_single_frame;
    double& displayW;
    double& displayH;
    double& displayX;
    double& displayY;
    GLFWwindow* window;

    // GUI-sim thread sharing
    std::atomic<int> simStep{0};
    Buffer buffers[2];
    std::atomic<int> readInd{0};
    std::atomic<bool> newFrame {false};
    std::atomic<bool> paused {false};
    std::atomic<bool> guiPaused {false};
    std::mutex swapMutex;
};

void simulate(Attractor& sim, globalState& shared, std::atomic<bool>& running, double dt, bool LOG_ENERGY, bool LOG_SIM_TIME);

void updateBuffer(globalState& shared, int ind, Points& points);

void keyCallback(GLFWwindow* w, int key, int sc, int action, int mods);

GLuint compile_shader(GLenum type, const char* src);

GLuint create_program(const std::string& vs_path, const std::string& fs_path);
