#include "main.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <thread>
#include <atomic>

#include <png.h>
#include <Eigen/Dense>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "attractor.h"
#include "gl_utils.h"

#include "consts.h"


using namespace std;
using namespace Consts;



// Current display scaling
double displayW = viewW;
double displayH = viewH;

// Current display shift
double displayX = 0;
double displayY = 0;


int main(int argc, char** argv) {
   // Runtime options
    bool save_frames = false;
    bool save_single_frame = false;

    // parse optional args
    for (int i=1;i<argc;i++) if ((string)argv[i] == "--save") save_frames = true;


    // Simulation/window state
    globalState state{save_frames, save_single_frame, displayW, displayH, displayX, displayY, nullptr};

    // Initialize simulation
    Attractor sim(N, viewW, viewH);

    string outdir = "outputFrames";
    if (!filesystem::exists(outdir)) filesystem::create_directory(outdir);

    // Runtime variables
    double dt = (double)1/FPS_TARGET * SPEED;
    int frame_idx = 0;
    double lastTime = glfwGetTime();
    int lastSavedFrame = -1;

    // Threading setup
    state.buffers[0] = Buffer(N);
    state.buffers[1] = Buffer(N);
    Points& initpoints = sim.getPoints();

    updateBuffer(state, 0, initpoints);

    // Start simulation thread
    atomic<bool> running{true};
    thread simThread(simulate, ref(sim), ref(state), ref(running), dt, LOG_ENERGY, LOG_SIM_TIME);


    // HEADLESS
    if constexpr (HEADLESS) {
        simThread.join();
        return 0;
    }


    // GUI
    GLFWwindow* window = initWindow(screenW, screenH, "Galaxy Sim");
    if (!window) return -1;
    state.window = window;


    float quadVerts[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    auto [quadVAO, quadVBO] = createStaticVAO(quadVerts, sizeof(quadVerts));

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint pxVBO   = attachVBO(0, 1, N);
    GLuint pyVBO   = attachVBO(1, 1, N);
    GLuint pzVBO   = attachVBO(2, 1, N);
    GLuint ageVBO  = attachVBO(3, 1, N);

    // Unbind VAO (important to prevent overwriting) and VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint program = createProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
    GLuint bgProgram = createProgram("shaders/bg_vertex.glsl", "shaders/bg_fragment.glsl");


    // define keybinds
    glfwSetWindowUserPointer(window, &state);
    glfwSetKeyCallback(window, keyCallback);

    GLint colorLoc          = glGetUniformLocation(program,         "color");
    GLint displaySizeLoc    = glGetUniformLocation(program,   "displaySize");
    GLint displayOffsetLoc  = glGetUniformLocation(program, "displayOffset");


    const Buffer* buffer = &state.buffers[0];

    cout << "Starting main loop (press ESC to quit, S to toggle saving, SPACE to pause/resume, P to save single frame)" << endl;
    int displayFrame = 1;
    while (!glfwWindowShouldClose(window)) {
        if(state.guiPaused) {
            this_thread::sleep_for(chrono::milliseconds(10));
            glfwPollEvents();
            double now = glfwGetTime();
            double elapsed = (now - lastTime) * 1000.0;
            lastTime = now;

            if (LOG_GUI_TIME && displayFrame % LOG_GUI_TIME_INTERVAL == 0) cout << fixed << setprecision(2) << elapsed << " ms of frametime" << endl;
            if (elapsed < FRAME_TARGET) this_thread::sleep_for(chrono::milliseconds((long long)(FRAME_TARGET - elapsed)));
            displayFrame += 1;
            continue;
        }
        // reuse data if nothing new
        if (state.newFrame.exchange(false)) {
            int ind;
            {
                lock_guard<mutex> lock(state.swapMutex);
                ind = state.readInd.load();
            }
            buffer = &state.buffers[ind];

            uploadBuffer(pxVBO,   buffer->px.data(),   sizeof(float) * buffer->N);
            uploadBuffer(pyVBO,   buffer->pz.data(),   sizeof(float) * buffer->N);
            uploadBuffer(pzVBO,   buffer->pz.data(),   sizeof(float) * buffer->N);
            uploadBuffer(ageVBO,  buffer->age.data(),  sizeof(float) * buffer->N);

        }

        // Draw background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawVAO(bgProgram, quadVAO, GL_TRIANGLE_STRIP, 4);

        // Draw points
        glUseProgram(program);
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);
        glUniform2f(displaySizeLoc, (float)displayW, (float)displayH);
        glUniform2f(displayOffsetLoc, (float)(displayX - 0.5f * (displayW - viewW)), (float)(displayY - 0.5f * (displayH - viewH)));
        drawVAO(program, vao, GL_POINTS, buffer->N);

        // Put to screen
        glfwSwapBuffers(window);
        // keybinds
        glfwPollEvents();

        // saving
        if (save_frames || save_single_frame) {
            int curFrame = state.simStep.load();
            if(curFrame != lastSavedFrame) {
                ostringstream ss;
                ss << outdir << "/frame_" << setw(5) << setfill('0') << frame_idx << ".png";
                saveFramebufferPNG(ss.str(), screenW, screenH);
                ++frame_idx;
                save_single_frame = false;
            }
        }

        // frame timing
        double now = glfwGetTime();
        double elapsed = (now - lastTime) * 1000.0;
        lastTime = now;

        if (LOG_GUI_TIME && displayFrame % LOG_GUI_TIME_INTERVAL == 0) cout << fixed << setprecision(2) << elapsed << " ms of frametime" << endl;
        if (elapsed < FRAME_TARGET) this_thread::sleep_for(chrono::milliseconds((long long)(FRAME_TARGET - elapsed)));
        displayFrame += 1;
    }

    running = false;
    simThread.join();

    glDeleteBuffers(1, &ageVBO);
    glDeleteBuffers(1, &pzVBO);
    glDeleteBuffers(1, &pyVBO);
    glDeleteBuffers(1, &pxVBO);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



void simulate(Attractor& sim, globalState& shared, atomic<bool>& running, double dt, bool LOG_ENERGY, bool LOG_SIM_TIME) {
    int step = 0;
    bool INFO_FLAG_ENERGY = false;
    while (running) {
        if(shared.paused) {
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }
        if (LOG_ENERGY) INFO_FLAG_ENERGY = (step % LOG_ENERGY_INTERVAL == 0);

        auto start = chrono::high_resolution_clock::now();
        bool done = sim.step(dt, step);
        auto end = chrono::high_resolution_clock::now();
        if(!done) {
            double elapsed = chrono::duration<double, milli>(end - start).count();

            Points& points = sim.getPoints();

            int writeInd = 1 - shared.readInd.load();

            updateBuffer(shared, writeInd, points);
            {
                lock_guard<mutex> lock(shared.swapMutex);
                shared.readInd.store(writeInd);
                shared.newFrame.store(true);
                shared.simStep++;
            }
            step++;
        } else {
            running = false;
            if (shared.window) glfwSetWindowShouldClose(shared.window, GLFW_TRUE);
            break;
        }

    }
}


void updateBuffer(globalState& shared, int ind, Points& points) {
    shared.buffers[ind].N = points.N;
    shared.buffers[ind].curInd = points.curInd;
    shared.buffers[ind].px = points.px;
    shared.buffers[ind].py = points.py;
    shared.buffers[ind].pz = points.pz;

    for(int i = 0; i < N; i++) {
        int dist = shared.buffers[ind].curInd - i;
        if(dist < 0) {
            dist += shared.buffers[ind].N;
        }
        shared.buffers[ind].age[i] = dist/(float)N;
    }
}


void keyCallback(GLFWwindow* w, int key, int sc, int action, int mods) {
    if (action != GLFW_PRESS) return;
    auto* s = (globalState*)glfwGetWindowUserPointer(w);
    switch (key) {
        case GLFW_KEY_ESCAPE:   glfwSetWindowShouldClose(w, GLFW_TRUE); break;
        case GLFW_KEY_Q:        glfwSetWindowShouldClose(w, GLFW_TRUE); break;

        case GLFW_KEY_SPACE:    s->paused = !s->paused.load(); break;
        case GLFW_KEY_G:        s->guiPaused = !s->guiPaused.load(); break;

        case GLFW_KEY_J:        s->save_frames = !s->save_frames; break;
        case GLFW_KEY_P:        s->save_single_frame = true; break;

        case GLFW_KEY_UP:       s->displayW *= 0.66; s->displayH *= 0.66; break;
        case GLFW_KEY_DOWN:     s->displayW *= 1.5;  s->displayH *= 1.5;  break;

        case GLFW_KEY_W:        s->displayY += 0.1 * s->displayH; break;
        case GLFW_KEY_A:        s->displayX += -0.1 * s->displayW; break;
        case GLFW_KEY_S:        s->displayY += -0.1 * s->displayH; break;
        case GLFW_KEY_D:        s->displayX += 0.1 * s->displayW; break;
    }
    cout << keyName(key) << " pressed" << endl;
}
