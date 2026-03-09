#include <SFML/Graphics.hpp>
#include <optional>
#include <algorithm>
#include <random>
#include <deque>
#include "board.h"
#include "pieces.h"
#include "pieceState.h"

void debug(sf::RenderWindow& window, std::string status){
    const sf::Font font("assets/font/times.ttf");
    sf::Text text(font, status);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);
    text.setPosition({10, 750});
    window.draw(text);
}

std::vector<int> pieceBag;
std::mt19937 gen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
std::deque<int> pieceQueue;

int generateNextPiece() {
    if (pieceBag.empty()) {
        pieceBag = {0, 1, 2, 3, 4, 5, 6};
        std::shuffle(pieceBag.begin(), pieceBag.end(), gen);
    }

    int nextPiece = pieceBag.back();
    pieceBag.pop_back();

    return nextPiece;
}

int getNextPiece() {
    pieceQueue.push_back(generateNextPiece());

    int nextPiece = pieceQueue.front();
    pieceQueue.pop_front();

    return nextPiece;
}

void drawBoard(sf::RenderWindow& window, sf::Texture& texture, Board& board, PieceState currentPiece, int holdPiece) {
    const int tileSize = 32;
    int xBuffer = 180;
    int yBuffer = 40;

    int previewXOffset = xBuffer + (board.width * tileSize) + 20;

    sf::RectangleShape rect({static_cast<float>(tileSize), static_cast<float>(tileSize)});
    rect.setFillColor(sf::Color::Black);
    rect.setOutlineColor(sf::Color(128, 128, 128));
    rect.setOutlineThickness(1.f);

    //Draw the board (y=0 at bottom: row 0 drawn at bottom of board area)
    for (int i = 0; i < board.height; i++) {
        for (int j = 0; j < board.width; j++) {
            float screenY = static_cast<float>((board.height - 1 - i) * tileSize + yBuffer);
            rect.setPosition({static_cast<float>(j * tileSize + xBuffer), screenY});
            window.draw(rect);
        }
    }

    //Draw pieces on board
    sf::Sprite block(texture);
    for (int i = 0; i < board.height; i++) {
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
    //Draw Current Piece (only blocks on board; y=0 at bottom)
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            int val = getPiece(currentPiece)[i][j];
            if(val){
                int gridRow = currentPiece.y - i;
                if(gridRow < 0 || gridRow >= board.height) continue;  // skip blocks above/below board
                float x = static_cast<float>((j + currentPiece.x) * tileSize + xBuffer);
                float screenY = static_cast<float>((board.height - 1 - gridRow) * tileSize + yBuffer);
                sf::IntRect textureRect({(val + 1) * tileSize, 0}, {tileSize, tileSize});
                piece.setTextureRect(textureRect);

                piece.setPosition({x, screenY});

                window.draw(piece);
            }
        }
    }

    //Draw the hold piece
    sf::Sprite hold(texture);
    if(holdPiece != -1){
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                int val = getPiece({holdPiece,0})[i][j];
                if(val){
                    float x = static_cast<float>(j * tileSize + xBuffer - 150);
                    float y = static_cast<float>(i * tileSize + yBuffer + 10);

                    sf::IntRect textureRect({(val + 1) * tileSize, 0}, {tileSize, tileSize});
                    hold.setTextureRect(textureRect);

                    hold.setPosition({x, y});

                    window.draw(hold);
                }
            }
        }
    }
    
    //Draw the ghost (y decreases downward; find lowest valid y)
    sf::Sprite ghost(texture);
    ghost.setColor(sf::Color(255, 255, 255, 102));
    int ghostY = currentPiece.y;
    for(;;ghostY--){
        if(!board.validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, ghostY})) break;
    }
    ghostY++;  // backtrack to last valid position

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            int val = getPiece(currentPiece)[i][j];
            if(val){
                int gridRow = ghostY - i;
                if(gridRow < 0 || gridRow >= board.height) continue;
                float x = static_cast<float>((j + currentPiece.x) * tileSize + xBuffer);
                float screenY = static_cast<float>((board.height - 1 - gridRow) * tileSize + yBuffer);

                sf::IntRect textureRect({(val + 1) * tileSize, 0}, {tileSize, tileSize});
                ghost.setTextureRect(textureRect);

                ghost.setPosition({x, screenY});

                window.draw(ghost);
            }
        }
    }

    //Draw the preview
    sf::Sprite previewSprite(texture);
    for (int k = 0; k < 5; k++) {
        int pieceID = pieceQueue[k];
        auto shape = getPiece({pieceID, 0});

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (shape[i][j]) {
                    float x = static_cast<float>(j * tileSize + previewXOffset);
                    float y = static_cast<float>(i * tileSize + yBuffer + 20 + (k * 100));

                    sf::IntRect textureRect({(shape[i][j] + 1) * tileSize, 0}, {tileSize, tileSize});
                    previewSprite.setTextureRect(textureRect);
                    previewSprite.setPosition({x, y});
                    
                    window.draw(previewSprite);
                }
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Tetris");
    window.setFramerateLimit(60);

    sf::Texture texture;
    if (!texture.loadFromFile("assets/img/tetrisBlocks.png")) {
        return -1;
    }

    Board board(10, 20);

    PieceState currentPiece(getNextPiece(), 0, 3, board.height - 1);  // spawn at top; y=0 is bottom
    for(int i = 0; i < 6; i++) {
        pieceQueue.push_back(getNextPiece());
    }

    int holdPiece = -1;
    bool held = false;

    float timeSinceLastDrop = 0;
    bool grounded = false;
    float timeOnGround = 0;

    bool showDebug = true;

    sf::Clock fpsClock;

    while (window.isOpen()) {
        sf::Time dt = fpsClock.restart();
        float fps = 1.0f / dt.asSeconds();
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(visibleArea));
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::L) {
                    currentPiece.x++;
                    if(!board.validPosition(currentPiece)){
                        currentPiece.x--;
                    } else if(board.validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, currentPiece.y - 1})){
                        grounded = false;  // y-1 is one row down (toward bottom; y=0 at bottom)
                    }
                }

                if (keyPressed->code == sf::Keyboard::Key::M) {
                    currentPiece.x--;
                    if(!board.validPosition(currentPiece)){
                        currentPiece.x++;
                    } else if(board.validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, currentPiece.y - 1})){
                        grounded = false;  // y-1 is one row down (toward bottom; y=0 at bottom)
                    }
                }

                if (keyPressed->code == sf::Keyboard::Key::K) {
                    currentPiece = board.rotatePiece(currentPiece, 1);
                }

                if (keyPressed->code == sf::Keyboard::Key::S) {
                    currentPiece = board.rotatePiece(currentPiece, -1);
                }

                if (keyPressed->code == sf::Keyboard::Key::LShift) {
                    currentPiece = board.rotatePiece(currentPiece, 2);
                }

                if (keyPressed->code == sf::Keyboard::Key::D) {
                    if(held) break;
                    if(holdPiece != -1){
                        int temp = holdPiece;
                        holdPiece = currentPiece.piece;
                        currentPiece.piece = temp;
                    } else {
                        holdPiece = currentPiece.piece;
                        currentPiece.piece = getNextPiece();
                    }
                    currentPiece.rotation = 0;
                    currentPiece.x = 3;
                    currentPiece.y = board.height - 1;  // spawn at top
                    grounded = false;
                    held = true;
                }

                if (keyPressed->code == sf::Keyboard::Key::V) {
                    for(;;currentPiece.y--){
                        if(!board.validPosition(currentPiece)){
                            currentPiece.y++;  // backtrack to last valid
                            board.placePiece({currentPiece.piece, currentPiece.rotation, currentPiece.x, currentPiece.y});
                            currentPiece.piece = getNextPiece();
                            currentPiece.rotation = 0;
                            currentPiece.x = 3;
                            currentPiece.y = board.height;
                            grounded = false;
                            held = false;
                            timeOnGround = 0;
                            board.clearLines();
                            break;
                        }
                    }
                }

                if (keyPressed->code == sf::Keyboard::Key::Slash) {
                    for(;;currentPiece.y--){
                        if(!board.validPosition(currentPiece)){
                            currentPiece.y++;  // backtrack to last valid
                            grounded = true;
                            break;
                        }
                    }
                }

                if (keyPressed->code == sf::Keyboard::Key::F8) {
                    showDebug = !showDebug;
                }

                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        timeSinceLastDrop += 60/fps;
        if(timeSinceLastDrop > 30){
            currentPiece.y--;
            if(!board.validPosition(currentPiece)){
                grounded = true;
                currentPiece.y++;
            }
            timeSinceLastDrop = 0;
        }

        if(grounded){
            timeOnGround += 60/fps;
            if(timeOnGround >= 100){
                board.placePiece(currentPiece);
                currentPiece.piece = getNextPiece();
                currentPiece.rotation = 0;
                currentPiece.x = 3;
                currentPiece.y = board.height;
                grounded = false;
                held = false;
                timeOnGround = 0;
                board.clearLines();
            }
        }

        if(showDebug){
            std::string status = "FPS: " + std::to_string(static_cast<int>(fps)) + 
                                    " | Piece X: " + std::to_string(currentPiece.x) + 
                                    ", Piece Y: " + std::to_string(currentPiece.y);
            debug(window, status);
        }

        drawBoard(window, texture, board, currentPiece, holdPiece);

        window.display();
    }

    return 0;
}
