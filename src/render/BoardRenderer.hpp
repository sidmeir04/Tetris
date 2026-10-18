#ifndef BOARD_RENDERER_HPP
#define BOARD_RENDERER_HPP

#include "render/BoardBackground.hpp"
#include <SFML/Graphics.hpp>

struct Board;
struct GameSettings;

void drawPiece(sf::RenderWindow& window, sf::Sprite& sprite, int piece, int rotation, int x, int y, int tileSize);
void drawBoard(sf::RenderWindow& window, sf::Texture& texture, BoardBackground& boardSprite, Board& board, GameSettings& settings);

#endif
