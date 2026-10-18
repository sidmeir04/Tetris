#ifndef KEY_BINDING_HPP
#define KEY_BINDING_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <windows.h>

struct KeyBinding {
    std::string action;
    int vkCode;
    sf::Keyboard::Key key;
    KeyBinding(std::string a, int v, sf::Keyboard::Key k) : action(a), vkCode(v), key(k) {}
};

inline int sfmlKeyToVKCode(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::A: return 'A';
        case sf::Keyboard::Key::B: return 'B';
        case sf::Keyboard::Key::C: return 'C';
        case sf::Keyboard::Key::D: return 'D';
        case sf::Keyboard::Key::E: return 'E';
        case sf::Keyboard::Key::F: return 'F';
        case sf::Keyboard::Key::G: return 'G';
        case sf::Keyboard::Key::H: return 'H';
        case sf::Keyboard::Key::I: return 'I';
        case sf::Keyboard::Key::J: return 'J';
        case sf::Keyboard::Key::K: return 'K';
        case sf::Keyboard::Key::L: return 'L';
        case sf::Keyboard::Key::M: return 'M';
        case sf::Keyboard::Key::N: return 'N';
        case sf::Keyboard::Key::O: return 'O';
        case sf::Keyboard::Key::P: return 'P';
        case sf::Keyboard::Key::Q: return 'Q';
        case sf::Keyboard::Key::R: return 'R';
        case sf::Keyboard::Key::S: return 'S';
        case sf::Keyboard::Key::T: return 'T';
        case sf::Keyboard::Key::U: return 'U';
        case sf::Keyboard::Key::V: return 'V';
        case sf::Keyboard::Key::W: return 'W';
        case sf::Keyboard::Key::X: return 'X';
        case sf::Keyboard::Key::Y: return 'Y';
        case sf::Keyboard::Key::Z: return 'Z';
        case sf::Keyboard::Key::LShift: return VK_SHIFT;
        case sf::Keyboard::Key::Slash: return VK_OEM_2;
        case sf::Keyboard::Key::F8: return VK_F8;
        case sf::Keyboard::Key::Escape: return VK_ESCAPE;
        default: return 0;
    }
}

inline std::string keyToString(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::A: return "A";
        case sf::Keyboard::Key::B: return "B";
        case sf::Keyboard::Key::C: return "C";
        case sf::Keyboard::Key::D: return "D";
        case sf::Keyboard::Key::E: return "E";
        case sf::Keyboard::Key::F: return "F";
        case sf::Keyboard::Key::G: return "G";
        case sf::Keyboard::Key::H: return "H";
        case sf::Keyboard::Key::I: return "I";
        case sf::Keyboard::Key::J: return "J";
        case sf::Keyboard::Key::K: return "K";
        case sf::Keyboard::Key::L: return "L";
        case sf::Keyboard::Key::M: return "M";
        case sf::Keyboard::Key::N: return "N";
        case sf::Keyboard::Key::O: return "O";
        case sf::Keyboard::Key::P: return "P";
        case sf::Keyboard::Key::Q: return "Q";
        case sf::Keyboard::Key::R: return "R";
        case sf::Keyboard::Key::S: return "S";
        case sf::Keyboard::Key::T: return "T";
        case sf::Keyboard::Key::U: return "U";
        case sf::Keyboard::Key::V: return "V";
        case sf::Keyboard::Key::W: return "W";
        case sf::Keyboard::Key::X: return "X";
        case sf::Keyboard::Key::Y: return "Y";
        case sf::Keyboard::Key::Z: return "Z";
        case sf::Keyboard::Key::LShift: return "Shift";
        case sf::Keyboard::Key::Slash: return "/";
        case sf::Keyboard::Key::F8: return "F8";
        case sf::Keyboard::Key::Escape: return "Esc";
        default: return "Unknown";
    }
}

#endif
