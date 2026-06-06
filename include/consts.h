#pragma once

namespace Consts {
    //
    // Functions
    //
    template<typename T> inline constexpr T sq(T x) { return x * x; }

    //
    // Physical/mathematical constants
    //
    constexpr float PI = 3.1415926;
    constexpr float SIGMA = 10;
    constexpr float RHO = 28;
    constexpr float BETA = 8.0f/3.0f;

    //
    // Simulation constants
    //

    constexpr int N = 100000;


    //
    // Engine constants
    //
    constexpr bool HEADLESS = false;
    constexpr int FPS_TARGET = 30;
    constexpr double FRAME_TARGET = 1000.0/FPS_TARGET;
    constexpr float SPEED = 0.005;


    // view
    constexpr double viewW = 30.0;
    constexpr double viewH = 30.0;


    // window parameters
    constexpr int screenW = 1000;
    constexpr int screenH = 1000;


    // basic logging
    constexpr bool LOG_GUI_TIME = false; 
    constexpr int LOG_GUI_TIME_INTERVAL = 30;
    constexpr bool LOG_SIM_TIME = false;
    constexpr int LOG_SIM_TIME_INTERVAL = 5;
    constexpr bool LOG_ENERGY = false; 
    constexpr int LOG_ENERGY_INTERVAL = 1;

    constexpr int MASS_DISPLAY_THRESHOLD = 5;
    constexpr float VISIBILITY = 0.1;
}
