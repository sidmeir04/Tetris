#include "render/BoardBackground.hpp"

BoardBackground createBoardBackground(int width, int height, int tileSize, float xBuffer, float yBuffer) {
    sf::Vector2u texSize(
        static_cast<unsigned int>(width * tileSize),
        static_cast<unsigned int>(height * tileSize)
    );

    sf::RenderTexture renderTexture(texSize);
    renderTexture.clear(sf::Color::Black);

    float fTileSize = static_cast<float>(tileSize);
    sf::RectangleShape cell(sf::Vector2f(fTileSize, fTileSize));
    cell.setFillColor(sf::Color::Transparent);
    cell.setOutlineColor(sf::Color(128, 128, 128));
    cell.setOutlineThickness(-1.f);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            cell.setPosition(sf::Vector2f(j * fTileSize, i * fTileSize));
            renderTexture.draw(cell);
        }
    }

    renderTexture.display();

    BoardBackground bg(std::move(renderTexture));
    bg.sprite.setPosition(sf::Vector2f(xBuffer, yBuffer));

    return bg;
}
