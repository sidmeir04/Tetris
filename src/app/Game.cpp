#include "app/Game.hpp"

#include "input/Actions.hpp"
#include "render/BoardRenderer.hpp"
#include "render/DebugOverlay.hpp"
#include "render/Histogram.hpp"

#include <memory>

Game::Game(sf::Texture loadedTexture)
    : board(10, 20),
      moveLeftState(DAS_DELAY, ARR_INTERVAL, -1),
      moveRightState(DAS_DELAY, ARR_INTERVAL, 1),
      texture(std::move(loadedTexture)),
      boardBG(createBoardBackground(board.width, board.height, board.tileSize, board.xBuffer, board.yBuffer)),
      inputThread(inputQueue, settings.gameplayBindings) {
    toggleBtn.setSize({110, 40});
    toggleBtn.setPosition({680, 10});
    toggleBtn.setFillColor(sf::Color::Blue);
    setupSettingsMenu();
}

void Game::setupSettingsMenu() {
    SettingsPage general;
    general.addElement(std::make_unique<IntInput>(menu.getFont(), "Frame Rate", sf::Vector2f{150, 100}, frameRate));
    general.addElement(std::make_unique<IntInput>(menu.getFont(), "DAS", sf::Vector2f{150, 150}, DASint));
    general.addElement(std::make_unique<IntInput>(menu.getFont(), "ARR", sf::Vector2f{150, 200}, ARRint));
    menu.addPage(std::move(general));

    SettingsPage gameplayControls;
    float y = 90.f;
    for (auto& binding : settings.gameplayBindings) {
        gameplayControls.addElement(std::make_unique<KeyBinder>(
            menu.getFont(), binding.action, sf::Vector2f{150, y}, binding));
        y += 45.f;
    }
    menu.addPage(std::move(gameplayControls));

    SettingsPage menuControls;
    y = 100.f;
    for (auto& binding : settings.menuBindings) {
        menuControls.addElement(std::make_unique<KeyBinder>(
            menu.getFont(), binding.action, sf::Vector2f{150, y}, binding));
        y += 50.f;
    }
    menu.addPage(std::move(menuControls));
}

void Game::handleWindowEvent(sf::RenderWindow& window, const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        window.close();
    }

    if (const auto* resized = event.getIf<sf::Event::Resized>()) {
        sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
        window.setView(sf::View(visibleArea));
    }

    menu.handleEvent(event);

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (nonGameplayKeysDown.insert(keyPressed->code).second) {
            for (std::size_t i = 0; i < settings.menuBindings.size(); ++i) {
                if (keyPressed->code == settings.menuBindings[i].key) {
                    handleMenuAction(settings.menuActionAt(i), window);
                }
            }
        }
    }
    if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        nonGameplayKeysDown.erase(keyReleased->code);
    }
    if (event.is<sf::Event::FocusLost>()) {
        nonGameplayKeysDown.clear();
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            if (toggleBtn.getGlobalBounds().contains(sf::Vector2f(click->position))) {
                menu.isOpen = !menu.isOpen;
            }
        }
    }
}

void Game::handlePressedAction(GameplayAction action, std::chrono::time_point<std::chrono::high_resolution_clock> keyPressTime) {
    switch (action) {
        case GameplayAction::MoveRight:
            if (!moveRightState.isPressed) {
                moveRightState.press(keyPressTime);
                moveLeftState.release();
                board.movePiece(board.currentPiece, 1);
            }
            break;
        case GameplayAction::MoveLeft:
            if (!moveLeftState.isPressed) {
                moveLeftState.press(keyPressTime);
                moveRightState.release();
                board.movePiece(board.currentPiece, -1);
            }
            break;
        case GameplayAction::RotateClockwise:
            board.rotatePiece(board.currentPiece, 1);
            if (board.currentPiece.grounded) falling.rotate();
            break;
        case GameplayAction::RotateCounterClockwise:
            board.rotatePiece(board.currentPiece, -1);
            if (board.currentPiece.grounded) falling.rotate();
            break;
        case GameplayAction::Rotate180:
            board.rotatePiece(board.currentPiece, 2);
            if (board.currentPiece.grounded) falling.rotate();
            break;
        case GameplayAction::HoldPiece:
            board.holdPiece(board.currentPiece);
            falling.reset();
            break;
        case GameplayAction::HardDrop:
            for (; board.validPosition(board.currentPiece, -1); board.currentPiece.y--);
            if (!board.placePiece(board.currentPiece)) gameOver = true;
            falling.reset();
            break;
        case GameplayAction::SoftDrop:
            for (; board.validPosition(board.currentPiece, -1); board.currentPiece.y--);
            board.currentPiece.grounded = true;
            downPressed = true;
            break;
        case GameplayAction::Undo:
            board.undoPiece();
            falling.timeOnGround = 0;
            falling.timeSinceLastDrop = 0;
            break;
        case GameplayAction::Count:
            break;
    }
}

void Game::handleReleasedAction(GameplayAction action, std::chrono::time_point<std::chrono::high_resolution_clock> keyPressTime) {
    switch (action) {
        case GameplayAction::MoveRight:
            board.updatePiecePosition(board.currentPiece, &moveRightState, keyPressTime);
            releaseTimings.push_back(static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(keyPressTime - moveRightState.startTime).count()));
            moveRightState.release();
            break;
        case GameplayAction::MoveLeft:
            board.updatePiecePosition(board.currentPiece, &moveLeftState, keyPressTime);
            releaseTimings.push_back(static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(keyPressTime - moveLeftState.startTime).count()));
            moveLeftState.release();
            break;
        case GameplayAction::SoftDrop:
            downPressed = false;
            break;
        default:
            break;
    }
}

void Game::processInputQueue() {
    QueuedInputEvent queuedEvent{};
    while (inputQueue.dequeue(queuedEvent)) {
        if (!board.running) continue;
        if (queuedEvent.pressed) handlePressedAction(queuedEvent.action, queuedEvent.timestamp);
        else handleReleasedAction(queuedEvent.action, queuedEvent.timestamp);
    }
}

void Game::handleMenuAction(MenuAction action, sf::RenderWindow& window) {
    switch (action) {
        case MenuAction::ResetGame:
            board.resetGame(10, 20);
            board.held = false;
            gameOver = false;
            falling.reset();
            moveLeftState.release();
            moveRightState.release();
            downPressed = false;
            break;
        case MenuAction::ToggleStatistics:
            displayData = !displayData;
            break;
        case MenuAction::ToggleDebugInfo:
            showDebug = !showDebug;
            break;
        case MenuAction::ExitGame:
            window.close();
            break;
        case MenuAction::ToggleGuide:
            settings.guideEnabled = !settings.guideEnabled;
            break;
        case MenuAction::PauseGame:
            board.running = !board.running;
            moveLeftState.release();
            moveRightState.release();
            downPressed = false;
            break;
        case MenuAction::Count:
            break;
    }
}

void Game::updateFalling(float frameLength) {
    if (downPressed) {
        for (; board.validPosition(board.currentPiece, -1); board.currentPiece.y--);
        board.currentPiece.grounded = true;
    }

    if (board.currentPiece.grounded) {
        if (!board.validPosition(board.currentPiece, -1)) {
            falling.timeOnGround += 60 / frameLength;
            if (falling.timeOnGround >= falling.maxTimeGrounded) {
                if (!board.placePiece(board.currentPiece)) gameOver = true;
                falling.reset();
            }
        } else board.currentPiece.grounded = false;
    } else {
        falling.timeSinceLastDrop += 60 / frameLength;
        if (falling.timeSinceLastDrop > 60) {
            board.currentPiece.y--;
            if (!board.validPosition(board.currentPiece, -1)) {
                board.currentPiece.grounded = true;
            }
            falling.timeSinceLastDrop = 0;
        }
    }
}

void Game::update(float frameLength) {
    if (board.running) {
        board.updatePiecePosition(board.currentPiece, &moveRightState, std::chrono::high_resolution_clock::now());
        board.updatePiecePosition(board.currentPiece, &moveLeftState, std::chrono::high_resolution_clock::now());

        updateFalling(frameLength);

        if (DASint != DAS_DELAY.count()) {
            DAS_DELAY = std::chrono::milliseconds(DASint);
            moveLeftState.DAS_DELAY = DAS_DELAY;
            moveRightState.DAS_DELAY = DAS_DELAY;
        }

        if (ARRint != ARR_INTERVAL.count()) {
            ARR_INTERVAL = std::chrono::milliseconds(ARRint);
            moveLeftState.ARR_INTERVAL = ARR_INTERVAL;
            moveRightState.ARR_INTERVAL = ARR_INTERVAL;
        }

        if (gameOver) {
            board.resetGame(board.width, board.height);
            gameOver = false;
        }
    }
}

void Game::applySettings(sf::RenderWindow& window) {
    if (frameRateRefrence != frameRate) {
        window.setFramerateLimit(frameRate);
        frameRateRefrence = frameRate;
    }
}

void Game::recordFrameTime(float frameLength) {
    frameTimes.push_back(frameLength);
    if (frameTimes.size() > 30) {
        float totalFps = 0.f;
        for (const float frameTime : frameTimes) {
            totalFps += frameTime;
        }
        fps = totalFps / static_cast<float>(frameTimes.size());
        frameTimes.clear();
    }
}

void Game::draw(sf::RenderWindow& window) {
    if (showDebug) {
        std::string status = "FPS: " + std::to_string(static_cast<int>(fps)) +
                             " | Piece X: " + std::to_string(board.currentPiece.x) +
                             ", Piece Y: " + std::to_string(board.currentPiece.y) +
                             ", Max Time Grounded: " + std::to_string(falling.maxTimeGrounded);
        debug(window, status);
    }

    if (displayData) {
        drawHistogram(window, releaseTimings, 3 * DAS_DELAY.count());
    } else {
        drawBoard(window, texture, boardBG, board, settings);
    }

    window.draw(toggleBtn);
    menu.draw(window);
}
