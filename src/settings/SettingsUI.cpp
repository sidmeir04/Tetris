#include "settings/SettingsUI.hpp"

#include <stdexcept>

Checkbox::Checkbox(const sf::Font& font, const std::string& text, sf::Vector2f pos, bool& val)
    : value(val), label(font, text, 20) {
    label.setPosition(pos);
    box.setSize({20.f, 20.f});
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color::White);
    box.setPosition({pos.x + 250.f, pos.y + 5.f});
}

void Checkbox::draw(sf::RenderWindow& window) {
    box.setFillColor(value ? sf::Color::Green : sf::Color::Transparent);
    window.draw(label);
    window.draw(box);
}

void Checkbox::handleEvent(const sf::Event& event) {
    if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseClick->button == sf::Mouse::Button::Left) {
            if (box.getGlobalBounds().contains(sf::Vector2f(mouseClick->position))) {
                value = !value;
            }
        }
    }
}

Slider::Slider(const sf::Font& font, const std::string& text, sf::Vector2f pos, float& val)
    : value(val), label(font, text, 20) {
    label.setPosition(pos);
    bar.setSize({200.f, 10.f});
    bar.setFillColor(sf::Color(80, 80, 80));
    bar.setPosition({pos.x + 250.f, pos.y + 12.f});
    handle.setSize({12.f, 24.f});
    handle.setFillColor(sf::Color::Red);
}

void Slider::draw(sf::RenderWindow& window) {
    handle.setPosition({bar.getPosition().x + (value * 2.f) - 6.f, bar.getPosition().y - 7.f});
    window.draw(label);
    window.draw(bar);
    window.draw(handle);
}

void Slider::handleEvent(const sf::Event& event) {
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

KeyBinder::KeyBinder(const sf::Font& font, const std::string& text, sf::Vector2f pos, KeyBinding& k)
    : keyBinding(k), label(font, text, 20), keyText(font, keyToString(k.key), 16) {
    label.setPosition(pos);
    box.setSize({120.f, 30.f});
    box.setFillColor(sf::Color(60, 60, 60));
    box.setPosition({pos.x + 250.f, pos.y});
}

void KeyBinder::draw(sf::RenderWindow& window) {
    box.setOutlineColor(binding ? sf::Color::Yellow : sf::Color::White);
    box.setOutlineThickness(1);
    keyText.setString(binding ? "Press key..." : keyToString(keyBinding.key));
    keyText.setPosition({box.getPosition().x + 5.f, box.getPosition().y + 3.f});
    window.draw(label);
    window.draw(box);
    window.draw(keyText);
}

void KeyBinder::handleEvent(const sf::Event& event) {
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

IntInput::IntInput(const sf::Font& font, const std::string& text, sf::Vector2f pos, int& val, int min, int max)
    : value(val), label(font, text, 20), valueText(font, std::to_string(val), 16), minValue(min), maxValue(max) {
    label.setPosition(pos);
    box.setSize({120.f, 30.f});
    box.setFillColor(sf::Color(60, 60, 60));
    box.setPosition({pos.x + 250.f, pos.y});
    inputBuffer = std::to_string(val);
}

void IntInput::draw(sf::RenderWindow& window) {
    box.setOutlineColor(editing ? sf::Color::Yellow : sf::Color::White);
    box.setOutlineThickness(1);
    valueText.setString(editing ? inputBuffer : std::to_string(value));
    valueText.setPosition({box.getPosition().x + 5.f, box.getPosition().y + 3.f});
    window.draw(label);
    window.draw(box);
    window.draw(valueText);
}

void IntInput::handleEvent(const sf::Event& event) {
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

void SettingsPage::addElement(std::unique_ptr<UIElement> el) {
    elements.push_back(std::move(el));
}

void SettingsPage::handleEvent(const sf::Event& event) {
    for (auto& el : elements) el->handleEvent(event);
}

void SettingsPage::draw(sf::RenderWindow& window) {
    for (auto& el : elements) el->draw(window);
}

SettingsMenu::SettingsMenu() {
    if (!font.openFromFile("assets/font/times.ttf")) throw std::runtime_error("Font not found");
    background.setSize({600.f, 450.f});
    background.setFillColor(sf::Color(20, 20, 20, 230));
    background.setOutlineThickness(2);
    background.setPosition({100.f, 75.f});
}

void SettingsMenu::addPage(SettingsPage page) {
    pages.push_back(std::move(page));
}

const sf::Font& SettingsMenu::getFont() const {
    return font;
}

void SettingsMenu::handleEvent(const sf::Event& event) {
    if (!isOpen) return;
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Right) currentPage = (currentPage + 1) % pages.size();
        if (keyPressed->code == sf::Keyboard::Key::Left) currentPage = (currentPage - 1 + (int)pages.size()) % pages.size();
    }
    pages[currentPage].handleEvent(event);
}

void SettingsMenu::draw(sf::RenderWindow& window) {
    if (!isOpen) return;
    window.draw(background);
    pages[currentPage].draw(window);
    sf::Text hint(font, "Page " + std::to_string(currentPage + 1) + "/" + std::to_string(pages.size()) + " (Arrow Keys)", 14);
    hint.setPosition({110.f, 495.f});
    window.draw(hint);
}
