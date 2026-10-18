#ifndef BOARD_BACKGROUND_HPP
#define BOARD_BACKGROUND_HPP

#include <SFML/Graphics.hpp>

struct BoardBackground {
    sf::RenderTexture renderTexture;
    sf::Sprite sprite;

    BoardBackground(sf::RenderTexture&& rTex)
        : renderTexture(std::move(rTex)), sprite(renderTexture.getTexture()) {}
};

BoardBackground createBoardBackground(int width, int height, int tileSize, float xBuffer, float yBuffer);

#endif
