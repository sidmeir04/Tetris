#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <cstddef>

enum class GameplayAction {
    MoveRight = 0,
    MoveLeft,
    RotateClockwise,
    RotateCounterClockwise,
    Rotate180,
    HoldPiece,
    HardDrop,
    SoftDrop,
    Undo,
    Count
};

enum class NonGameplayAction {
    ResetGame = 0,
    ToggleStatistics,
    ToggleDebugInfo,
    ExitGame,
    ToggleGuide,
    PauseGame,
    Count
};

inline constexpr std::size_t gameplayActionCount() {
    return static_cast<std::size_t>(GameplayAction::Count);
}

inline constexpr std::size_t nonGameplayActionCount() {
    return static_cast<std::size_t>(NonGameplayAction::Count);
}

#endif
