#include <SFML/Graphics.hpp>
#include <optional>
#include <array>
#include <map>
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

void drawHistogram(sf::RenderWindow& window, const std::vector<int>& data, int max) {
    if (data.empty() || max <= 0) return;

    // 1. Count frequencies in buckets of 10
    int numBucketsTotal = (max + 9) / 10;
    if (numBucketsTotal <= 0) return;
    
    std::vector<int> counts(numBucketsTotal, 0);
    for (int x : data) {
        if (x < max) {
            int bucket = x / 10;
            if (bucket < numBucketsTotal) counts[bucket]++;
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

    for (int val = 0; val < numBucketsTotal; val++) {
        int freq = counts[val];
        if (freq == 0) continue;
        float h = (static_cast<float>(freq) / maxFreq) * chartSize.y;
        float xPos = val * barWidth;

        sf::Color barColor = sf::Color::Blue;

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
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Tetris");
    window.setFramerateLimit(120);

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

    const std::chrono::milliseconds DAS_DELAY(100); 
    const std::chrono::milliseconds ARR_INTERVAL(0);

    InputState moveLeftState(DAS_DELAY,ARR_INTERVAL,-1);
    InputState moveRightState(DAS_DELAY,ARR_INTERVAL,1);

    const int TRACKED_KEYS = 12;
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
        {'I', sf::Keyboard::Key::I},
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
                    board.currentPiece.reset(board.currentPiece.piece, 0, 3, board.height + 2, false);
                    timeOnGround = 0;
                    timeSinceLastDrop = 0;
                    board.held = true;
                }

                if (queuedEvent.key == sf::Keyboard::Key::V) {
                    for(;;board.currentPiece.y--){
                        if(!board.validPosition(board.currentPiece)){
                            board.placePiece({board.currentPiece.piece, board.currentPiece.rotation, board.currentPiece.x, board.currentPiece.y+1});
                            timeOnGround = 0;
                            timeSinceLastDrop = 0;
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

                if (queuedEvent.key == sf::Keyboard::Key::I) {
                    displayData = !displayData;
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
        if(board.currentPiece.grounded){
            board.currentPiece.y--;
            if(board.validPosition(board.currentPiece)){
                timeOnGround = 0;
                timeSinceLastDrop = 0;
                board.currentPiece.grounded = false;
            } else {
                timeOnGround += 60/fps;
                if(timeOnGround >= 100){
                    board.placePiece(board.currentPiece);
                    timeOnGround = 0;
                    timeSinceLastDrop = 0;
                }
            }
            board.currentPiece.y++;
        }

        if(timeSinceLastDrop > 30){
            board.currentPiece.y--;
            if(!board.validPosition(board.currentPiece)){
                board.currentPiece.grounded = true;
                board.currentPiece.y++;
            }
            timeSinceLastDrop = 0;
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

        

        window.display();
    }

    inputThreadRunning.store(false);
    if (inputThread.joinable()) {
        inputThread.join();
    }

    return 0;
}
