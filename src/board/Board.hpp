#ifndef BOARD_HPP
#define BOARD_HPP

#include "board/History.hpp"
#include "guide/Guide.hpp"
#include "input/InputState.hpp"
#include "piece/PieceState.hpp"

#include <chrono>
#include <deque>
#include <random>
#include <stack>
#include <vector>

struct Board {
    int width;
    int height;
    std::vector<std::vector<int>> grid;
    std::stack<BoardSnapShot> moveHistory;
    BoardSnapShot currentMove;

    std::vector<int> pieceBag;
    std::mt19937 gen;
    std::deque<int> pieceQueue;
    PieceState currentPiece;
    int heldPiece = -1;
    int piecesPlaced = 0;
    bool held = false;
    Guide guide;
    bool running = true;

    const int tileSize = 32;
    int xBuffer = 200;
    int yBuffer = 80;

    Board(int w, int h);

    int generateNextPiece();
    int getNextPiece();
    bool validPosition(const PieceState& state, int yOffset = 0);
    void resetBoard(int w, int h);
    void resetGame(int w, int h);
    std::vector<int> clearLines();
    bool placePiece(const PieceState& state);
    void undoPiece();
    void movePiece(PieceState& currentPiece, int movement);
    void updatePiecePosition(PieceState& currentPiece, InputState* input, std::chrono::time_point<std::chrono::high_resolution_clock> time);
    void rotatePiece(PieceState& currentPiece, int rotation);
    void holdPiece(PieceState& currentPiece);
    std::vector<std::vector<int>> getGuideBlock();
};

#endif
