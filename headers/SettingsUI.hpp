#ifndef SETTINGS_UI_HPP
#define SETTINGS_UI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <windows.h>

// Convert SFML key codes to Windows virtual key codes
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

// Get a string name for a key
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

class UIElement {
public:
    virtual ~UIElement() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
};

class Checkbox : public UIElement {
    sf::RectangleShape box;
    sf::Text label;
    bool& value;
public:
    Checkbox(const sf::Font& font, const std::string& text, sf::Vector2f pos, bool& val) : value(val), label(font, text, 20) {
        label.setPosition(pos);
        box.setSize({20.f, 20.f});
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color::White);
        box.setPosition({pos.x + 250.f, pos.y + 5.f});
    }

    void draw(sf::RenderWindow& window) override {
        box.setFillColor(value ? sf::Color::Green : sf::Color::Transparent);
        window.draw(label);
        window.draw(box);
    }

    void handleEvent(const sf::Event& event) override {
        if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseClick->button == sf::Mouse::Button::Left) {
                if (box.getGlobalBounds().contains(sf::Vector2f(mouseClick->position))) {
                    value = !value;
                }
            }
        }
    }
};

class Slider : public UIElement {
    sf::RectangleShape bar, handle;
    sf::Text label;
    float& value;
public:
    Slider(const sf::Font& font, const std::string& text, sf::Vector2f pos, float& val) : value(val), label(font, text, 20) {
        label.setPosition(pos);
        bar.setSize({200.f, 10.f});
        bar.setFillColor(sf::Color(80, 80, 80));
        bar.setPosition({pos.x + 250.f, pos.y + 12.f});
        handle.setSize({12.f, 24.f});
        handle.setFillColor(sf::Color::Red);
    }

    void draw(sf::RenderWindow& window) override {
        handle.setPosition({bar.getPosition().x + (value * 2.f) - 6.f, bar.getPosition().y - 7.f});
        window.draw(label);
        window.draw(bar);
        window.draw(handle);
    }

    void handleEvent(const sf::Event& event) override {
        if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseClick->button == sf::Mouse::Button::Left) {
                if (bar.getGlobalBounds().contains(sf::Vector2f(mouseClick->position))) {
                    value = ((mouseClick->position.x - bar.getPosition().x) / 200.f) * 100.f;
                    if (value < 0) value = 0;
                    if (value > 100) value = 100;
                }
            }
        }
    }
};

struct KeyBinding {
    std::string action;
    int vkCode;
    sf::Keyboard::Key key;
    KeyBinding(std::string a, int v, sf::Keyboard::Key k) : action(a), vkCode(v), key(k){}
};

class KeyBinder : public UIElement {
    sf::RectangleShape box;
    sf::Text label;
    sf::Text keyText;
    KeyBinding& keyBinding;
    bool binding = false;
public:
    KeyBinder(const sf::Font& font, const std::string& text, sf::Vector2f pos, KeyBinding& k) : keyBinding(k), label(font, text, 20), keyText(font, keyToString(k.key), 16) {
        label.setPosition(pos);
        box.setSize({120.f, 30.f});
        box.setFillColor(sf::Color(60, 60, 60));
        box.setPosition({pos.x + 250.f, pos.y});
    }

    void draw(sf::RenderWindow& window) override {
        box.setOutlineColor(binding ? sf::Color::Yellow : sf::Color::White);
        box.setOutlineThickness(1);
        keyText.setString(binding ? "Press key..." : keyToString(keyBinding.key));
        keyText.setPosition({box.getPosition().x + 5.f, box.getPosition().y + 3.f});
        window.draw(label);
        window.draw(box);
        window.draw(keyText);
    }

    void handleEvent(const sf::Event& event) override {
        if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (box.getGlobalBounds().contains(sf::Vector2f(mouseClick->position))) binding = true;
        }
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (binding) {
                keyBinding.key = keyPressed->code;
                keyBinding.vkCode = sfmlKeyToVKCode(keyPressed->code);
                binding = false;
            }
        }
    }
};

class IntInput : public UIElement {
    sf::RectangleShape box;
    sf::Text label;
    sf::Text valueText;
    int& value;
    bool editing = false;
    std::string inputBuffer;
    int minValue;
    int maxValue;
public:
    IntInput(const sf::Font& font, const std::string& text, sf::Vector2f pos, int& val, int min = 0, int max = 9999) 
        : value(val), label(font, text, 20), valueText(font, std::to_string(val), 16), minValue(min), maxValue(max) {
        label.setPosition(pos);
        box.setSize({120.f, 30.f});
        box.setFillColor(sf::Color(60, 60, 60));
        box.setPosition({pos.x + 250.f, pos.y});
        inputBuffer = std::to_string(val);
    }

    void draw(sf::RenderWindow& window) override {
        box.setOutlineColor(editing ? sf::Color::Yellow : sf::Color::White);
        box.setOutlineThickness(1);
        valueText.setString(editing ? inputBuffer : std::to_string(value));
        valueText.setPosition({box.getPosition().x + 5.f, box.getPosition().y + 3.f});
        window.draw(label);
        window.draw(box);
        window.draw(valueText);
    }

    void handleEvent(const sf::Event& event) override {
        if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseClick->button == sf::Mouse::Button::Left) {
                if (box.getGlobalBounds().contains(sf::Vector2f(mouseClick->position))) {
                    editing = true;
                    inputBuffer = std::to_string(value);
                }
            }
        }
        
        if (editing) {
            if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    try {
                        int newVal = std::stoi(inputBuffer);
                        if (newVal >= minValue && newVal <= maxValue) {
                            value = newVal;
                        }
                    } catch (...) {
                        // Invalid input, keep old value
                    }
                    editing = false;
                    inputBuffer = std::to_string(value);
                } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    editing = false;
                    inputBuffer = std::to_string(value);
                }
            }
            
            if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
                if (textEntered->unicode >= '0' && textEntered->unicode <= '9') {
                    if (inputBuffer.length() < 5) {
                        inputBuffer += static_cast<char>(textEntered->unicode);
                    }
                } else if (textEntered->unicode == '\b' && !inputBuffer.empty()) {
                    inputBuffer.pop_back();
                }
            }
        }
    }
};

class SettingsPage {
    std::vector<std::unique_ptr<UIElement>> elements;
public:
    void addElement(std::unique_ptr<UIElement> el) { elements.push_back(std::move(el)); }
    void handleEvent(const sf::Event& event) { for (auto& el : elements) el->handleEvent(event); }
    void draw(sf::RenderWindow& window) { for (auto& el : elements) el->draw(window); }
};

class SettingsMenu {
    std::vector<SettingsPage> pages;
    sf::RectangleShape background;
    sf::Font font;
    int currentPage = 0;
public:
    bool isOpen = false;

    SettingsMenu() {
        if (!font.openFromFile("assets/font/times.ttf")) throw std::runtime_error("Font not found");
        background.setSize({600.f, 450.f});
        background.setFillColor(sf::Color(20, 20, 20, 230));
        background.setOutlineThickness(2);
        background.setPosition({100.f, 75.f});
    }

    void addPage(SettingsPage page) { pages.push_back(std::move(page)); }
    const sf::Font& getFont() const { return font; }

    void handleEvent(const sf::Event& event) {
        if (!isOpen) return;
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Right) currentPage = (currentPage + 1) % pages.size();
            if (keyPressed->code == sf::Keyboard::Key::Left) currentPage = (currentPage - 1 + (int)pages.size()) % pages.size();
        }
        pages[currentPage].handleEvent(event);
    }

    void draw(sf::RenderWindow& window) {
        if (!isOpen) return;
        window.draw(background);
        pages[currentPage].draw(window);
        sf::Text hint(font, "Page " + std::to_string(currentPage + 1) + "/" + std::to_string(pages.size()) + " (Arrow Keys)", 14);
        hint.setPosition({110.f, 495.f});
        window.draw(hint);
    }
};

#endif