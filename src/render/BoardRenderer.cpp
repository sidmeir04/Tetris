#include "render/BoardRenderer.hpp"
#include "board/Board.hpp"
#include "piece/Pieces.hpp"
#include "settings/GameSettings.hpp"

#include <algorithm>

void drawPiece(sf::RenderWindow& window, sf::Sprite& sprite, int piece, int rotation, int x, int y, int tileSize) {
    std::vector<std::vector<int>> pieceGrid = getPiece({piece, rotation});
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int val = pieceGrid[i][j];
            if (val) {
                sf::IntRect textureRect({(val + 1) * 32, 0}, {tileSize, tileSize});
                sprite.setTextureRect(textureRect);

                sprite.setPosition({static_cast<float>(j * tileSize + x), static_cast<float>(i * tileSize + y)});

                window.draw(sprite);
            }
        }
    }
}

void drawBoard(sf::RenderWindow& window, sf::Texture& texture, BoardBackground& boardSprite, Board& board, GameSettings& settings) {
    PieceState currentPiece = board.currentPiece;
    int heldPiece = board.heldPiece;
    const int tileSize = board.tileSize;
    int xBuffer = board.xBuffer;
    int yBuffer = board.yBuffer;

    int previewXOffset = xBuffer + (board.width * tileSize) + 20;

    window.draw(boardSprite.sprite);

    if (board.guide.active) {
        sf::Sprite guideBlock(texture);
        guideBlock.setColor(sf::Color(255, 255, 255, 75));
        std::vector<std::vector<int>> guideBlocks = board.getGuideBlock();
        for (int i = 0; i < guideBlocks.size(); i++) {
            for (int j = 0; j < guideBlocks[i].size(); j++) {
                int val = guideBlocks[i][j];
                if (val > 0) {
                    float screenY = static_cast<float>((board.height - 1 - i) * tileSize + yBuffer);
                    sf::IntRect textureRect({(val + 1) * tileSize, 0}, {tileSize, tileSize});
                    guideBlock.setTextureRect(textureRect);
                    guideBlock.setPosition({static_cast<float>(j * tileSize + xBuffer), screenY});
                    window.draw(guideBlock);
                }
            }
        }
    }

    sf::Sprite block(texture);
    for (int i = 0; i < board.height + 5; i++) {
        for (int j = 0; j < board.width; j++) {
            int val = board.grid[i][j];
            if (val > 0) {
                float screenY = static_cast<float>((board.height - 1 - i) * tileSize + yBuffer);
                sf::IntRect textureRect({(val + 1) * tileSize, 0}, {tileSize, tileSize});
                block.setTextureRect(textureRect);
                block.setPosition({static_cast<float>(j * tileSize + xBuffer), screenY});
                window.draw(block);
            }
        }
    }

    sf::Sprite piece(texture);
    drawPiece(window, piece, currentPiece.piece, currentPiece.rotation, currentPiece.x * tileSize + xBuffer, (board.height - 1 - currentPiece.y) * tileSize + yBuffer, tileSize);

    if (heldPiece != -1) {
        sf::Sprite hold(texture);
        drawPiece(window, hold, heldPiece, 0, xBuffer - 150, yBuffer + 10, tileSize);
    }

    sf::Sprite ghost(texture);
    ghost.setColor(sf::Color(255, 255, 255, 130));
    int ghostY = currentPiece.y;
    for (;; ghostY--) {
        if (!board.validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, ghostY})) break;
    }
    ghostY++;
    drawPiece(window, ghost, currentPiece.piece, currentPiece.rotation, currentPiece.x * tileSize + xBuffer, (board.height - 1 - ghostY) * tileSize + yBuffer, tileSize);

    sf::Sprite previewSprite(texture);
    for (int k = 0; k < std::min(static_cast<int>(board.pieceQueue.size()), 5); k++) {
        int pieceID = board.pieceQueue[k];
        drawPiece(window, previewSprite, pieceID, 0, previewXOffset, yBuffer + 20 + (k * 100), tileSize);
    }
}
