#ifndef GAME_SETTINGS_HPP
#define GAME_SETTINGS_HPP

#include "input/Actions.hpp"
#include "settings/KeyBinding.hpp"

#include <vector>

struct GameSettings {
    float masterVolume = 80.f;
    bool vSync = true;

    std::vector<KeyBinding> gameplayBindings = {
        {"Move Right", 'L', sf::Keyboard::Key::L},
        {"Move Left", 'M', sf::Keyboard::Key::M},
        {"Rotate Clockwise", 'K', sf::Keyboard::Key::K},
        {"Rotate Counter-Clockwise", 'S', sf::Keyboard::Key::S},
        {"Rotate 180", VK_SHIFT, sf::Keyboard::Key::LShift},
        {"Hold Piece", 'D', sf::Keyboard::Key::D},
        {"Hard Drop", 'V', sf::Keyboard::Key::V},
        {"Soft Drop", VK_OEM_2, sf::Keyboard::Key::Slash},
        {"Undo", 'Z', sf::Keyboard::Key::Z},
    };

    std::vector<KeyBinding> nonGameplayBindings = {
        {"Reset Game", 'R', sf::Keyboard::Key::R},
        {"Toggle Statistics", 'I', sf::Keyboard::Key::I},
        {"Toggle Debug Info", VK_F8, sf::Keyboard::Key::F8},
        {"Exit Game", VK_ESCAPE, sf::Keyboard::Key::Escape},
        {"Toggle Guide", 'G', sf::Keyboard::Key::G},
        {"Pause Game", 'P', sf::Keyboard::Key::P},
    };

    GameplayAction gameplayActionAt(std::size_t index) const {
        return static_cast<GameplayAction>(index);
    }

    NonGameplayAction nonGameplayActionAt(std::size_t index) const {
        return static_cast<NonGameplayAction>(index);
    }
};

#endif
