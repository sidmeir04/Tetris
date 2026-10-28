#ifndef INPUT_QUEUE_HPP
#define INPUT_QUEUE_HPP

#include "input/Actions.hpp"

#include <array>
#include <chrono>
#include <mutex>

struct GameplayInputEvent {
    GameplayAction action;
    bool pressed;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
};

struct GameplayInputBuffer {
    static constexpr std::size_t CAPACITY = 13;
    std::array<GameplayInputEvent, CAPACITY> events{};
    std::size_t head = 0;
    std::size_t size = 0;
    std::mutex mutex;

    void enqueue(const GameplayInputEvent& event) {
        std::lock_guard<std::mutex> lock(mutex);
        if (size < CAPACITY) {
            events[(head + size) % CAPACITY] = event;
            size++;
            return;
        }
        events[head] = event;
        head = (head + 1) % CAPACITY;
    }

    bool dequeue(GameplayInputEvent& event) {
        std::lock_guard<std::mutex> lock(mutex);
        if (size == 0) {
            return false;
        }
        event = events[head];
        head = (head + 1) % CAPACITY;
        size--;
        return true;
    }
};

#endif
