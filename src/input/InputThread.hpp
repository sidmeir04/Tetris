#ifndef INPUT_THREAD_HPP
#define INPUT_THREAD_HPP

#include "input/InputQueue.hpp"
#include "settings/KeyBinding.hpp"

#include <atomic>
#include <thread>
#include <vector>

class InputThread {
    InputEventQueue& queue;
    const std::vector<KeyBinding>& trackedKeys;
    std::atomic<bool> running{true};
    std::vector<char> previousState;
    std::thread thread;

public:
    InputThread(InputEventQueue& queue, const std::vector<KeyBinding>& trackedKeys);
    ~InputThread();

    InputThread(const InputThread&) = delete;
    InputThread& operator=(const InputThread&) = delete;
};

#endif
