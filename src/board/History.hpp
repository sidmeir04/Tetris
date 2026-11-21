#ifndef BOARD_HISTORY_HPP
#define BOARD_HISTORY_HPP

#include "piece/PieceState.hpp"
#include <deque>
#include <vector>

struct BoardSnapShot {
    PieceState placedPiece;

    bool linesCleared;
    std::vector<int> lineIndexes;
    std::vector<std::vector<int>> lineInformation;

    bool held;
    int heldPiece;

    bool capturedState = false;
    PieceState currentPieceBefore;
    std::deque<int> pieceQueueBefore;
    std::vector<int> pieceBagBefore;
    int heldPieceBefore = -1;
    bool heldBefore = false;
    int piecesPlacedBefore = 0;

    BoardSnapShot()
        : placedPiece(0, 0, 0, 0, false),
          linesCleared(false),
          held(false),
          heldPiece(-1) {}

    void reset() {
        placedPiece.reset(0, 0, 0, 0, false);
        linesCleared = false;
        lineIndexes.clear();
        lineInformation.clear();
        held = false;
        heldPiece = -1;
        capturedState = false;
        currentPieceBefore.reset(0, 0, 0, 0, false);
        pieceQueueBefore.clear();
        pieceBagBefore.clear();
        heldPieceBefore = -1;
        heldBefore = false;
        piecesPlacedBefore = 0;
    }
};

#endif
