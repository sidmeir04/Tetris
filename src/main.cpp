#include "app/Game.hpp"

#include <SFML/Graphics.hpp>
#include <optional>
#include <utility>

int main() {
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "Tetris");
    sf::Texture texture;
    if (!texture.loadFromFile("assets/img/tetrisBlocks.png")) {
        return -1;
    }

    Game game(std::move(texture));
    window.setFramerateLimit(game.frameRate);

    sf::Clock fpsClock;
    while (window.isOpen()) {
        sf::Time frameTime = fpsClock.restart();
        float frameLength = 1.0f / frameTime.asSeconds();
        game.recordFrameTime(frameLength);

        while (const std::optional event = window.pollEvent()) {
            game.handleWindowEvent(window, *event);
        }

        game.processInputQueue();
        window.clear(sf::Color(30, 30, 30));
        game.update(frameLength);
        game.applySettings(window);
        game.draw(window);
        window.display();
    }

    return 0;
}
