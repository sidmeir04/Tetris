#ifndef PIECE_HANDLING
#define PIECE_HANDLING

#include <chrono>

struct InputState {
    bool isPressed = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastRepeat;
    std::chrono::milliseconds DAS_DELAY;
    std::chrono::milliseconds ARR_INTERVAL;
    bool dasTriggered = false;
    int movement;

    InputState(std::chrono::milliseconds das, std::chrono::milliseconds arr, int move)
        : DAS_DELAY(das), ARR_INTERVAL(arr), movement(move) {}

    void press(const std::chrono::time_point<std::chrono::high_resolution_clock>& now){
        isPressed = true;
        dasTriggered = false;
        startTime = now;
        lastRepeat = now;
    }

    void release(){
        isPressed = false;
        dasTriggered = false;
    }
};

#endif