#include <SFML/Graphics.hpp>
#include <optional>
#include <array>
#include <map>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <windows.h>
#include <iostream>
#include "board.h"
#include "pieces.h"
#include "pieceHandling.h"
#include "SettingsUI.hpp"

struct QueuedInputEvent {
    sf::Keyboard::Key key;
    bool pressed;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
};

struct GameSettings {
    float masterVolume = 80.f;
    bool vSync = true;
    std::vector<KeyBinding> keyBindings = {
        {"Move Right", 'L', sf::Keyboard::Key::L},
        {"Move Left", 'M', sf::Keyboard::Key::M},
        {"Rotate Clockwise", 'K', sf::Keyboard::Key::K},
        {"Rotate Counter-Clockwise", 'S', sf::Keyboard::Key::S},
        {"Rotate 180", VK_SHIFT, sf::Keyboard::Key::LShift},
        {"Hold Piece", 'D', sf::Keyboard::Key::D},
        {"Hard Drop", 'V', sf::Keyboard::Key::V},
        {"Soft Drop", VK_OEM_2, sf::Keyboard::Key::Slash},
        {"Reset Game", 'R', sf::Keyboard::Key::R},
        {"Toggle Statistics", 'I', sf::Keyboard::Key::I},
        {"Toggle Debug Info", VK_F8, sf::Keyboard::Key::F8},
        {"Exit Game", VK_ESCAPE, sf::Keyboard::Key::Escape}
    };
};

struct InputEventQueue {
    static constexpr std::size_t CAPACITY = 12;
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

void debug(sf::RenderWindow& window, std::string status){
    const sf::Font font("assets/font/times.ttf");
    sf::Text text(font, status);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);
    text.setPosition({10, 750});
    window.draw(text);
}

void drawPiece(sf::RenderWindow& window, sf::Sprite& sprite, int piece, int rotation, int x, int y, int tileSize){
    std::vector<std::vector<int>> pieceGrid = getPiece({piece, rotation});
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            int val = pieceGrid[i][j];
            if(val){
                sf::IntRect textureRect({(val + 1) * 32, 0}, {tileSize, tileSize});
                sprite.setTextureRect(textureRect);

                sprite.setPosition({static_cast<float>(j * tileSize + x), static_cast<float>(i * tileSize + y)});

                window.draw(sprite);
            }
        }
    }
}

void drawBoard(sf::RenderWindow& window, sf::Texture& texture, Board& board) {
    PieceState currentPiece = board.currentPiece;
    int holdPiece = board.holdPiece;
    const int tileSize = 32;
    int xBuffer = 200;
    int yBuffer = 80;

    int previewXOffset = xBuffer + (board.width * tileSize) + 20;

    sf::RectangleShape rect({static_cast<float>(tileSize), static_cast<float>(tileSize)});
    rect.setFillColor(sf::Color::Black);
    rect.setOutlineColor(sf::Color(128, 128, 128));
    rect.setOutlineThickness(1.f);

    //Draw the board
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

    //Draw Current Piece
    sf::Sprite piece(texture);
    drawPiece(window, piece, currentPiece.piece, currentPiece.rotation, currentPiece.x*tileSize + xBuffer, (board.height - 1 - currentPiece.y) * tileSize + yBuffer, tileSize);

    //Draw the hold piece
    if(holdPiece != -1){
        sf::Sprite hold(texture);
        drawPiece(window, hold, holdPiece, 0, xBuffer - 150, yBuffer + 10, tileSize);
    }
    
    //Draw the ghost
    sf::Sprite ghost(texture);
    ghost.setColor(sf::Color(255, 255, 255, 102));
    int ghostY = currentPiece.y;
    for(;;ghostY--){
        if(!board.validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, ghostY})) break;
    }
    ghostY++;
    drawPiece(window, ghost, currentPiece.piece, currentPiece.rotation, currentPiece.x*tileSize + xBuffer, (board.height - 1 - ghostY) * tileSize + yBuffer, tileSize);

    //Draw the preview
    sf::Sprite previewSprite(texture);
    for (int k = 0; k < 5; k++) {
        int pieceID = board.pieceQueue[k];
        drawPiece(window, previewSprite, pieceID, 0, previewXOffset, yBuffer + 20 + (k * 100), tileSize);
    }
}

void drawHistogram(sf::RenderWindow& window, const std::vector<int>& data, int max) {
    if (data.empty() || max <= 0) return;

    // 1. Count frequencies in buckets of 10
    int numBucketsTotal = (max + 10) / 10;
    std::vector<int> counts(numBucketsTotal, 0);

    for (int x : data) {
        if (x < max && x > 0) {
            int bucket = x / 10;
            counts[bucket]++;
        }
    }

    // 2. Find max frequency
    int maxFreq = 0;
    for (int freq : counts) {
        if (freq > maxFreq) maxFreq = freq;
    }
    if (maxFreq == 0) return;

    // Chart constants
    const float padding = 60.f;
    auto windowSize = window.getSize();
    if (windowSize.x <= padding * 2 || windowSize.y <= padding * 2) return;
    
    const sf::Vector2f chartSize(windowSize.x - padding * 2, windowSize.y - padding * 2);
    const sf::Vector2f offset(padding, windowSize.y - padding);

    // 3. Create geometry
    sf::VertexArray va(sf::PrimitiveType::Triangles);
    float barWidth = chartSize.x / numBucketsTotal;

    sf::Color barColor = sf::Color::Cyan;

    for (int val = 0; val < numBucketsTotal; val++) {
        int freq = counts[val];
        if (freq == 0) continue;
        float h = (static_cast<float>(freq) / maxFreq) * chartSize.y;
        float xPos = val * barWidth;


        // Standard 6-vertex quad
        va.append({{offset.x + xPos, offset.y}, barColor});
        va.append({{offset.x + xPos, offset.y - h}, barColor});
        va.append({{offset.x + xPos + barWidth, offset.y - h}, barColor});

        va.append({{offset.x + xPos, offset.y}, barColor});
        va.append({{offset.x + xPos + barWidth, offset.y - h}, barColor});
        va.append({{offset.x + xPos + barWidth, offset.y}, barColor});
    }

    window.draw(va);

    // 4. Draw Labels (X and Y Axis)
    const sf::Font font("assets/font/times.ttf");
    sf::Text label(font, "");
    label.setCharacterSize(14);
    label.setFillColor(sf::Color::White);

    // Y-Axis (Frequencies) - Draw 5 markers
    for (int i = 0; i <= 5; ++i) {
        int val = (maxFreq * i) / 5;
        float yPos = offset.y - ((static_cast<float>(val) / maxFreq) * chartSize.y);
        
        label.setString(std::to_string(val));
        label.setPosition({offset.x - 40.f, yPos - 10.f});
        window.draw(label);
    }

    // X-Axis (Values) - Label 5 markers with ranges
    std::vector<int> xMarkers;
    int numMarkers = 5;
    for (int i = 0; i < numMarkers; i++) {
        int val = i * (numBucketsTotal - 1) / (numMarkers - 1);
        xMarkers.push_back(val);
    }
    for (int val : xMarkers) {
        float xPos = offset.x + val * barWidth;
        int start = val * 10;
        int end = start + 9;
        std::string labelStr = std::to_string(start) + "-" + std::to_string(end);
        label.setString(labelStr);
        label.setPosition({xPos, offset.y + 10.f});
        window.draw(label);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "Tetris");
    int frameRate = 120;
    int frameRateRefrence = frameRate;
    window.setFramerateLimit(frameRate);

    sf::Texture texture;
    if (!texture.loadFromFile("assets/img/tetrisBlocks.png")) {
        return -1;
    }

    Board board(10, 20);

    std::vector<int> releaseTimings;

    bool downPressed = false;

    bool displayData = false;

    float timeSinceLastDrop = 0;
    float timeOnGround = 0;

    bool showDebug = true;

    std::chrono::milliseconds DAS_DELAY(90);
    int DASint = 90;
    std::chrono::milliseconds ARR_INTERVAL(0);
    int ARRint = 0;

    InputState moveLeftState(DAS_DELAY,ARR_INTERVAL,-1);
    InputState moveRightState(DAS_DELAY,ARR_INTERVAL,1);

    GameSettings data;
    SettingsMenu menu;

    SettingsPage general;
    general.addElement(std::make_unique<IntInput>(menu.getFont(), "Frame Rate", sf::Vector2f{150, 100}, frameRate));
    general.addElement(std::make_unique<IntInput>(menu.getFont(), "DAS", sf::Vector2f{150, 150}, DASint));
    general.addElement(std::make_unique<IntInput>(menu.getFont(), "ARR", sf::Vector2f{150, 200}, ARRint));
    menu.addPage(std::move(general));

    SettingsPage controls;
    float y = 100.f;
    for(size_t i = 0; i < data.keyBindings.size(); i++){
        controls.addElement(std::make_unique<KeyBinder>(menu.getFont(), data.keyBindings[i].action, sf::Vector2f{150, y}, data.keyBindings[i]));
        y += 50;
    }
    menu.addPage(std::move(controls));

    sf::RectangleShape toggleBtn({110, 40});
    toggleBtn.setPosition({680, 10});
    toggleBtn.setFillColor(sf::Color::Blue);

    InputEventQueue inputQueue;
    std::atomic<bool> inputThreadRunning{true};
    const std::vector<KeyBinding>& trackedKeys = data.keyBindings;
    std::array<bool, 12> previousState{};

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

            menu.handleEvent(*event);

            // Local logic for the toggle button
            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    if (toggleBtn.getGlobalBounds().contains(sf::Vector2f(click->position))) {
                        menu.isOpen = !menu.isOpen;
                    }
                }
            }
        }

        QueuedInputEvent queuedEvent{};
        while (inputQueue.dequeue(queuedEvent)) {
            auto keyPressTime = queuedEvent.timestamp;
            if (queuedEvent.pressed) {
                if (queuedEvent.key == data.keyBindings[0].key) {
                    if (!moveRightState.isPressed) {
                        moveRightState.press(keyPressTime);
                        moveLeftState.release();
                        board.movePiece(board.currentPiece, 1);
                    }
                }
                if (queuedEvent.key == data.keyBindings[1].key) {
                    if (!moveLeftState.isPressed) {
                        moveLeftState.press(keyPressTime);
                        moveRightState.release();
                        board.movePiece(board.currentPiece, -1);
                    }
                }

                if (queuedEvent.key == data.keyBindings[2].key) {
                    board.rotatePiece(board.currentPiece, 1);
                }

                if (queuedEvent.key == data.keyBindings[3].key) {
                    board.rotatePiece(board.currentPiece, -1);
                }

                if (queuedEvent.key == data.keyBindings[4].key) {
                    board.rotatePiece(board.currentPiece, 2);
                }

                if (queuedEvent.key == data.keyBindings[5].key) {
                    if(board.held) continue;
                    if(board.holdPiece != -1){
                        int temp = board.holdPiece;
                        board.holdPiece = board.currentPiece.piece;
                        board.currentPiece.piece = temp;
                    } else {
                        board.holdPiece = board.currentPiece.piece;
                        board.currentPiece.piece = board.getNextPiece();
                    }
                    board.currentPiece.reset(board.currentPiece.piece, 0, 3, board.height + 2, false);
                    timeOnGround = 0;
                    timeSinceLastDrop = 0;
                    board.held = true;
                }

                if (queuedEvent.key == data.keyBindings[6].key) {
                    for(;board.validPosition(board.currentPiece, -1);board.currentPiece.y--);
                    board.placePiece(board.currentPiece);
                    timeOnGround = 0;
                    timeSinceLastDrop = 0;
                }

                if (queuedEvent.key == data.keyBindings[7].key) {
                    for(;board.validPosition(board.currentPiece, -1);board.currentPiece.y--);
                    board.currentPiece.grounded = true;
                    downPressed = true;
                }

                if (queuedEvent.key == data.keyBindings[8].key) {
                    board.resetGame(10, 20);
                    board.held = false;
                    timeOnGround = 0;
                    timeSinceLastDrop = 0;
                }

                if (queuedEvent.key == data.keyBindings[9].key) {
                    displayData = !displayData;
                }

                if (queuedEvent.key == data.keyBindings[10].key) {
                    showDebug = !showDebug;
                }

                if (queuedEvent.key == data.keyBindings[11].key) {
                    window.close();
                }
            } else {
                if (queuedEvent.key == sf::Keyboard::Key::L) {
                    board.updatePiecePosition(board.currentPiece, &moveRightState, keyPressTime);
                    releaseTimings.push_back(static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(keyPressTime - moveRightState.startTime).count()));
                    moveRightState.release();
                };
                if (queuedEvent.key == sf::Keyboard::Key::M) {
                    board.updatePiecePosition(board.currentPiece, &moveLeftState, keyPressTime);
                    releaseTimings.push_back(static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(keyPressTime - moveRightState.startTime).count()));
                    moveLeftState.release();
                };
                if (queuedEvent.key == sf::Keyboard::Key::Slash) downPressed = false;
            }
        }

        board.updatePiecePosition(board.currentPiece, &moveRightState, std::chrono::high_resolution_clock::now());
        board.updatePiecePosition(board.currentPiece, &moveLeftState, std::chrono::high_resolution_clock::now());

        window.clear(sf::Color(30, 30, 30));

        if(downPressed){
            for(;board.validPosition(board.currentPiece, -1);board.currentPiece.y--);
            board.currentPiece.grounded = true;
        }
        
        timeSinceLastDrop += 60/fps;
        if(board.currentPiece.grounded){
            if(board.validPosition(board.currentPiece, -1)){
                timeOnGround = 0;
                timeSinceLastDrop = 0;
                board.currentPiece.grounded = false;
            }
            timeOnGround += 60/fps;
            if(timeOnGround >= 100){
                board.placePiece(board.currentPiece);
                timeOnGround = 0;
                timeSinceLastDrop = 0;
            }
        }

        if((timeSinceLastDrop > 60) && (!board.currentPiece.grounded)){
            board.currentPiece.y--;
            if(!board.validPosition(board.currentPiece, -1)){
                board.currentPiece.grounded = true;
            }
            timeSinceLastDrop = 0;
        }

        if(DASint != DAS_DELAY.count()){
            DAS_DELAY = std::chrono::milliseconds(DASint);
            moveLeftState.DAS_DELAY = DAS_DELAY;
            moveRightState.DAS_DELAY = DAS_DELAY;
        }

        if(ARRint != ARR_INTERVAL.count()){
            ARR_INTERVAL = std::chrono::milliseconds(ARRint);
            moveLeftState.ARR_INTERVAL = ARR_INTERVAL;
            moveRightState.ARR_INTERVAL = ARR_INTERVAL;
        }

        if(frameRateRefrence != frameRate){
            window.setFramerateLimit(frameRate);
            frameRateRefrence = frameRate;
        }

        if(showDebug) {
            std::string status = "FPS: " + std::to_string(static_cast<int>(fps)) + 
                                    " | Piece X: " + std::to_string(board.currentPiece.x) + 
                                    ", Piece Y: " + std::to_string(board.currentPiece.y) +
                                    ", Grounded: " + std::to_string(board.currentPiece.grounded);
            debug(window, status);
        }


        if(displayData) {
            drawHistogram(window, releaseTimings, 3*DAS_DELAY.count());
        } else {
            drawBoard(window, texture, board);
        }

        window.draw(toggleBtn);
        menu.draw(window);

        

        window.display();
    }

    inputThreadRunning.store(false);
    if (inputThread.joinable()) {
        inputThread.join();
    }

    return 0;
}
