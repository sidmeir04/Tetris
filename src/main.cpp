#include <SFML/Graphics.hpp>
#include <optional>
#include <array>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <windows.h>
#include "board.h"
#include "pieces.h"
#include "pieceHandling.h"

struct QueuedInputEvent {
    sf::Keyboard::Key key;
    bool pressed;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
};

struct InputEventQueue {
    static constexpr std::size_t CAPACITY = 8;
    std::array<QueuedInputEvent, CAPACITY> events{};
    std::size_t head = 0;
    std::size_t size = 0;
    std::mutex mutex;

    void enqueue(const QueuedInputEvent& event) {
        std::lock_guard<std::mutex> lock(mutex);
        if (size < CAPACITY) {
            events[(head + size) % CAPACITY] = event;
            size++;
            return;
        }
        // Keep latest real-time events when queue is full.
        events[head] = event;
        head = (head + 1) % CAPACITY;
    }

    bool dequeue(QueuedInputEvent& event) {
        std::lock_guard<std::mutex> lock(mutex);
        if (size == 0) {
            return false;
        }
        event = events[head];
        head = (head + 1) % CAPACITY;
        size--;
        return true;
    }
};

struct KeyBinding {
    int vkCode;
    sf::Keyboard::Key key;
};

void debug(sf::RenderWindow& window, std::string status){
    const sf::Font font("assets/font/times.ttf");
    sf::Text text(font, status);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);
    text.setPosition({10, 750});
    window.draw(text);
}

void drawBoard(sf::RenderWindow& window, sf::Texture& texture, Board& board) {
    PieceState currentPiece = board.currentPiece;
    int holdPiece = board.holdPiece;
    const int tileSize = 32;
    int xBuffer = 180;
    int yBuffer = 80;

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
    for (int i = 0; i < board.height+5; i++) {
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
                if(gridRow < 0) continue;
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
        int pieceID = board.pieceQueue[k];
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
    window.setFramerateLimit(240);

    sf::Texture texture;
    if (!texture.loadFromFile("assets/img/tetrisBlocks.png")) {
        return -1;
    }

    Board board(10, 20);

    bool downPressed = false;

    float timeSinceLastDrop = 0;
    float timeOnGround = 0;

    bool showDebug = true;

    const std::chrono::milliseconds DAS_DELAY(100); 
    const std::chrono::milliseconds ARR_INTERVAL(0);

    InputState moveLeftState(DAS_DELAY,ARR_INTERVAL,-1);
    InputState moveRightState(DAS_DELAY,ARR_INTERVAL,1);

    const int TRACKED_KEYS = 11;
    InputEventQueue inputQueue;
    std::atomic<bool> inputThreadRunning{true};
    std::array<KeyBinding, TRACKED_KEYS> trackedKeys = {{
        {'L', sf::Keyboard::Key::L},
        {'M', sf::Keyboard::Key::M},
        {'K', sf::Keyboard::Key::K},
        {'S', sf::Keyboard::Key::S},
        {'D', sf::Keyboard::Key::D},
        {'V', sf::Keyboard::Key::V},
        {'R', sf::Keyboard::Key::R},
        {VK_OEM_2, sf::Keyboard::Key::Slash},
        {VK_SHIFT, sf::Keyboard::Key::LShift},
        {VK_F8, sf::Keyboard::Key::F8},
        {VK_ESCAPE, sf::Keyboard::Key::Escape}
    }};
    std::array<bool, TRACKED_KEYS> previousState{};

    std::thread inputThread([&]() {
        while (inputThreadRunning.load()) {
            for (std::size_t i = 0; i < trackedKeys.size(); i++) {
                const bool isDown = (GetAsyncKeyState(trackedKeys[i].vkCode) & 0x8000) != 0;
                if (isDown != previousState[i]) {
                    previousState[i] = isDown;
                    inputQueue.enqueue({trackedKeys[i].key, isDown, std::chrono::high_resolution_clock::now()});
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

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
        }

        QueuedInputEvent queuedEvent{};
        while (inputQueue.dequeue(queuedEvent)) {
            auto keyPressTime = queuedEvent.timestamp;
            if (queuedEvent.pressed) {
                if (queuedEvent.key == sf::Keyboard::Key::L) {
                    if (!moveRightState.isPressed) {
                        moveRightState.press(keyPressTime);
                        moveLeftState.release();
                        board.currentPiece = board.movePiece(board.currentPiece, 1);
                    }
                }
                if (queuedEvent.key == sf::Keyboard::Key::M) {
                    if (!moveLeftState.isPressed) {
                        moveLeftState.press(keyPressTime);
                        moveRightState.release();
                        board.currentPiece = board.movePiece(board.currentPiece, -1);
                    }
                }

                if (queuedEvent.key == sf::Keyboard::Key::K) {
                    board.currentPiece = board.rotatePiece(board.currentPiece, 1);
                }

                if (queuedEvent.key == sf::Keyboard::Key::S) {
                    board.currentPiece = board.rotatePiece(board.currentPiece, -1);
                }

                if (queuedEvent.key == sf::Keyboard::Key::LShift) {
                    board.currentPiece = board.rotatePiece(board.currentPiece, 2);
                }

                if (queuedEvent.key == sf::Keyboard::Key::D) {
                    if(board.held) continue;
                    if(board.holdPiece != -1){
                        int temp = board.holdPiece;
                        board.holdPiece = board.currentPiece.piece;
                        board.currentPiece.piece = temp;
                    } else {
                        board.holdPiece = board.currentPiece.piece;
                        board.currentPiece.piece = board.getNextPiece();
                    }
                    board.currentPiece.reset(board.currentPiece.piece, 0, 3, board.height + 1, false);
                    timeOnGround = 0;
                    timeSinceLastDrop = 0;
                    board.held = true;
                }

                if (queuedEvent.key == sf::Keyboard::Key::V) {
                    for(;;board.currentPiece.y--){
                        if(!board.validPosition(board.currentPiece)){
                            board.currentPiece.y++;
                            board.placePiece({board.currentPiece.piece, board.currentPiece.rotation, board.currentPiece.x, board.currentPiece.y});
                            board.currentPiece.reset(board.getNextPiece(), 0, 3, board.height+1, false);
                            board.held = false;
                            timeOnGround = 0;
                            timeSinceLastDrop = 0;
                            board.clearLines();
                            break;
                        }
                    }
                }

                if (queuedEvent.key == sf::Keyboard::Key::Slash) {
                    downPressed = true;
                }

                if (queuedEvent.key == sf::Keyboard::Key::R) {
                    board.resetGame(10, 20);
                    board.held = false;
                    timeOnGround = 0;
                    timeSinceLastDrop = 0;
                }

                if (queuedEvent.key == sf::Keyboard::Key::F8) {
                    showDebug = !showDebug;
                }

                if (queuedEvent.key == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            } else {
                if (queuedEvent.key == sf::Keyboard::Key::L) {
                    board.updatePiecePosition(board.currentPiece, &moveRightState, keyPressTime);
                    moveRightState.release();
                };
                if (queuedEvent.key == sf::Keyboard::Key::M) {
                    board.updatePiecePosition(board.currentPiece, &moveLeftState, keyPressTime);
                    moveLeftState.release();
                };
                if (queuedEvent.key == sf::Keyboard::Key::Slash) downPressed = false;
            }
        }

        board.currentPiece = board.updatePiecePosition(board.currentPiece, &moveRightState, std::chrono::high_resolution_clock::now());
        board.currentPiece = board.updatePiecePosition(board.currentPiece, &moveLeftState, std::chrono::high_resolution_clock::now());

        window.clear(sf::Color(30, 30, 30));

        if(downPressed){
            for(;;board.currentPiece.y--){
                if(!board.validPosition(board.currentPiece)){
                    board.currentPiece.y++;
                    board.currentPiece.grounded = true;
                    break;
                }
            }
        }

        timeSinceLastDrop += 60/fps;
        if(timeSinceLastDrop > 30){
            board.currentPiece.y--;
            if(!board.validPosition(board.currentPiece)){
                board.currentPiece.grounded = true;
                board.currentPiece.y++;
            }
            timeSinceLastDrop = 0;
        }

        if(board.currentPiece.grounded){
            timeOnGround += 60/fps;
            if(timeOnGround >= 100){
                board.placePiece(board.currentPiece);
                board.currentPiece.piece = board.getNextPiece();
                board.currentPiece.reset(board.currentPiece.piece, 0, 3, board.height + 1, false);
                board.held = false;
                timeOnGround = 0;
                timeSinceLastDrop = 0;
                board.clearLines();
            }
        }

        if(showDebug){
            std::string status = "FPS: " + std::to_string(static_cast<int>(fps)) + 
                                    " | Piece X: " + std::to_string(board.currentPiece.x) + 
                                    ", Piece Y: " + std::to_string(board.currentPiece.y);
            debug(window, status);
        }

        drawBoard(window, texture, board);

        window.display();
    }

    inputThreadRunning.store(false);
    if (inputThread.joinable()) {
        inputThread.join();
    }

    return 0;
}
