#ifndef SETTINGS_UI_HPP
#define SETTINGS_UI_HPP

#include "settings/KeyBinding.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

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
    Checkbox(const sf::Font& font, const std::string& text, sf::Vector2f pos, bool& val);
    void draw(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
};

class Slider : public UIElement {
    sf::RectangleShape bar, handle;
    sf::Text label;
    float& value;
public:
    Slider(const sf::Font& font, const std::string& text, sf::Vector2f pos, float& val);
    void draw(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
};

class KeyBinder : public UIElement {
    sf::RectangleShape box;
    sf::Text label;
    sf::Text keyText;
    KeyBinding& keyBinding;
    bool binding = false;
public:
    KeyBinder(const sf::Font& font, const std::string& text, sf::Vector2f pos, KeyBinding& k);
    void draw(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
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
    IntInput(const sf::Font& font, const std::string& text, sf::Vector2f pos, int& val, int min = 0, int max = 9999);
    void draw(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
};

class SettingsPage {
    std::vector<std::unique_ptr<UIElement>> elements;
public:
    void addElement(std::unique_ptr<UIElement> el);
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);
};

class SettingsMenu {
    std::vector<SettingsPage> pages;
    sf::RectangleShape background;
    sf::Font font;
    int currentPage = 0;
public:
    bool isOpen = false;

    SettingsMenu();
    void addPage(SettingsPage page);
    const sf::Font& getFont() const;
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);
};

#endif
