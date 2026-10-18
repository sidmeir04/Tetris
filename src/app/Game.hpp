#ifndef GAME_HPP
#define GAME_HPP

#include "app/FallingState.hpp"
#include "board/Board.hpp"
#include "input/Actions.hpp"
#include "input/InputQueue.hpp"
#include "input/InputState.hpp"
#include "input/InputThread.hpp"
#include "render/BoardBackground.hpp"
#include "settings/GameSettings.hpp"
#include "settings/SettingsUI.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <set>
#include <vector>

class Game {
public:
    int frameRate = 120;
    int frameRateRefrence = 120;

    Board board;
    FallingState falling;
    GameSettings settings;
    SettingsMenu menu;
    SettingsMenu gameSettings;

    std::chrono::milliseconds DAS_DELAY{90};
    int DASint = 90;
    std::chrono::milliseconds ARR_INTERVAL{0};
    int ARRint = 0;

    InputState moveLeftState;
    InputState moveRightState;

    sf::Texture texture;
    BoardBackground boardBG;
    sf::RectangleShape toggleBtn;

    std::vector<int> releaseTimings;
    bool downPressed = false;
    bool displayData = false;
    bool gameOver = false;
    bool showDebug = true;

    InputEventQueue inputQueue;
    InputThread inputThread;
    std::set<sf::Keyboard::Key> nonGameplayKeysDown;

    std::vector<float> frameTimes;
    float fps = 0.f;

    explicit Game(sf::Texture loadedTexture);

    void setupSettingsMenu();
    void handleWindowEvent(sf::RenderWindow& window, const sf::Event& event);
    void processInputQueue();
    void update(float frameLength);
    void applySettings(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void recordFrameTime(float frameLength);

private:
    void handlePressedAction(GameplayAction action, std::chrono::time_point<std::chrono::high_resolution_clock> keyPressTime);
    void handleReleasedAction(GameplayAction action, std::chrono::time_point<std::chrono::high_resolution_clock> keyPressTime);
    void handleMenuAction(MenuAction action, sf::RenderWindow& window);
    void updateFalling(float frameLength);
};

#endif
