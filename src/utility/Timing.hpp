#ifndef TIMING_HPP
#define TIMING_HPP

#include <chrono>
#include <iostream>

struct Timer {
    const std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastTime;

    Timer() :
        startTime(std::chrono::steady_clock::now()),
        lastTime(startTime) {}

    void printTotalElapsedTime() {
        const auto elapsed = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - startTime
        ).count();
        std::cout << "execution time: " << elapsed << " ms" << std::endl;
        lastTime = std::chrono::steady_clock::now();
    }

    void printElapsedTime() {
        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration<double, std::milli>(
            now - lastTime
        ).count();
        std::cout << "execution time: " << elapsed << " ms" << std::endl;
        lastTime = now;
    }
};

#endif