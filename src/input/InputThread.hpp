#ifndef INPUT_THREAD_HPP
#define INPUT_THREAD_HPP

#include "input/InputQueue.hpp"
#include "settings/KeyBinding.hpp"

#include <atomic>
#include <thread>
#include <vector>

class InputThread {
    GameplayInputBuffer& gameplayInputBuffer;
    const std::vector<KeyBinding>& gameplayBindings;
    std::atomic<bool> running{true};
    std::vector<char> previousState;
    std::thread thread;

public:
    InputThread(GameplayInputBuffer& gameplayInputBuffer, const std::vector<KeyBinding>& gameplayBindings);
    ~InputThread();

    InputThread(const InputThread&) = delete;
    InputThread& operator=(const InputThread&) = delete;
};

#endif
