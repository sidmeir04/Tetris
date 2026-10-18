#ifndef BOARD_HISTORY_HPP
#define BOARD_HISTORY_HPP

#include "piece/PieceState.hpp"
#include <vector>

struct BoardSnapShot {
    PieceState placedPiece;

    bool linesCleared;
    std::vector<int> lineIndexes;
    std::vector<std::vector<int>> lineInformation;

    bool held;
    int heldPiece;

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
    }
};

#endif
