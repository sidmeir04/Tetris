#include "input/InputThread.hpp"
#include "input/Actions.hpp"

#include <chrono>
#include <windows.h>

InputThread::InputThread(GameplayInputBuffer& inputBuffer, const std::vector<KeyBinding>& bindings)
    : gameplayInputBuffer(inputBuffer), gameplayBindings(bindings), previousState(bindings.size(), 0) {
    thread = std::thread([this]() {
        while (running.load()) {
            for (std::size_t i = 0; i < gameplayBindings.size(); i++) {
                const bool isDown = (GetAsyncKeyState(gameplayBindings[i].vkCode) & 0x8000) != 0;
                if (isDown != static_cast<bool>(previousState[i])) {
                    previousState[i] = static_cast<char>(isDown);
                    gameplayInputBuffer.enqueue({
                        static_cast<GameplayAction>(i),
                        isDown,
                        std::chrono::high_resolution_clock::now()
                    });
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}

InputThread::~InputThread() {
    running.store(false);
    if (thread.joinable()) {
        thread.join();
    }
}
