#include "board/Board.hpp"
#include "guide/GuidePatterns.hpp"
#include "piece/Pieces.hpp"
#include "piece/Srs.hpp"

#include <algorithm>
#include <chrono>

Board::Board(int w, int h)
    : width(w),
      height(h),
      grid(h + 5, std::vector<int>(w, 0)),
      gen(static_cast<unsigned int>(
          std::chrono::system_clock::now().time_since_epoch().count())),
      currentPiece(0, 0, 0, 0, false),
      guide(SDPCguide1, "Single Double PC") {
    currentPiece = PieceState(getNextPiece(), 0, 3, height - 1, false);
    for (int i = 0; i < 6; i++) {
        pieceQueue.push_back(generateNextPiece());
    }
}

int Board::generateNextPiece() {
    if (pieceBag.empty()) {
        pieceBag = {0, 1, 2, 3, 4, 5, 6};
        std::shuffle(pieceBag.begin(), pieceBag.end(), gen);
    }

    int nextPiece = pieceBag.back();
    pieceBag.pop_back();

    return nextPiece;
}

int Board::getNextPiece() {
    if (pieceQueue.size() < 6) {
        pieceQueue.push_back(generateNextPiece());
    }

    int nextPiece = pieceQueue.front();
    pieceQueue.pop_front();

    return nextPiece;
}

bool Board::validPosition(const PieceState& state, int yOffset) {
    std::vector<std::vector<int>> piece = getPiece(state);
    int x;
    int gridRow;
    for (int i = 0; i < piece.size(); i++) {
        for (int j = 0; j < piece[0].size(); j++) {
            if (piece[i][j]) {
                x = state.x + j;
                gridRow = state.y - i + yOffset;
                if ((x < 0) || (x >= width) || (gridRow < 0) || grid[gridRow][x]) return false;
            }
        }
    }
    return true;
}

void Board::resetBoard(int w, int h) {
    width = w;
    height = h;
    grid.clear();
    grid.assign(h + 5, std::vector<int>(w, 0));
    while (!moveHistory.empty()) moveHistory.pop();
}

void Board::resetGame(int w, int h) {
    resetBoard(w, h);
    pieceQueue.clear();
    pieceBag.clear();
    for (int i = 0; i < 6; i++) {
        pieceQueue.push_back(getNextPiece());
    }
    currentPiece.reset(currentPiece.piece, 0, 3, height + 1, false);
    currentPiece.piece = getNextPiece();
    heldPiece = -1;
    piecesPlaced = 0;
    guide.reset();
}

std::vector<int> Board::clearLines() {
    std::vector<int> linesCleared = {};
    for (int i = height + 4; i >= 0; i--) {
        bool isFull = true;
        for (int j = 0; j < width; j++) {
            if (grid[i][j] == 0) {
                isFull = false;
                break;
            }
        }
        if (isFull) {
            std::vector<int> clearedLine = *(grid.begin() + i);
            grid.erase(grid.begin() + i);
            grid.insert(grid.begin() + (height + 4), std::vector<int>(width, 0));
            currentMove.lineInformation.push_back(clearedLine);
            linesCleared.push_back(i);
            i++;
        }
    }

    guide.clearLines(linesCleared, piecesPlaced);
    currentMove.lineIndexes = linesCleared;
    currentMove.linesCleared = true;

    return linesCleared;
}

bool Board::placePiece(const PieceState& state) {
    std::vector<std::vector<int>> piece = getPiece(state);
    int x;
    int gridRow;
    for (int i = 0; i < piece.size(); i++) {
        for (int j = 0; j < piece[0].size(); j++) {
            if (piece[i][j]) {
                x = state.x + j;
                gridRow = state.y - i;
                if (gridRow >= 0) {
                    grid[gridRow][x] = piece[i][j];
                }
            }
        }
    }
    currentMove.placedPiece = currentPiece;
    clearLines();
    moveHistory.push(currentMove);
    currentMove.reset();
    currentPiece.reset(getNextPiece(), 0, 3, height + 2, false);
    held = false;
    if (!validPosition(currentPiece, -1)) currentPiece.grounded = true;
    piecesPlaced++;
    if (!validPosition(currentPiece)) return false;
    return true;
}

void Board::undoPiece() {
    if (moveHistory.empty()) return;

    BoardSnapShot lastMove = moveHistory.top();
    if (lastMove.linesCleared) {
        for (auto it = lastMove.lineIndexes.rbegin(); it != lastMove.lineIndexes.rend(); ++it) {
            grid.insert(grid.begin() + *it, lastMove.lineInformation.back());
            lastMove.lineInformation.pop_back();
        }
    }
    int x;
    int gridRow;
    std::vector<std::vector<int>> piece = getPiece(lastMove.placedPiece);
    for (int i = 0; i < piece.size(); i++) {
        for (int j = 0; j < piece[0].size(); j++) {
            if (piece[i][j]) {
                x = lastMove.placedPiece.x + j;
                gridRow = lastMove.placedPiece.y - i;
                if (gridRow >= 0) {
                    grid[gridRow][x] = 0;
                }
            }
        }
    }
    pieceQueue.push_front(currentPiece.piece);
    if (!lastMove.held) {
        currentPiece.reset(lastMove.placedPiece.piece, 0, 3, height + 2, false);
        held = false;
    } else if (lastMove.heldPiece == -1) {
        heldPiece = -1;
        currentPiece.reset(lastMove.placedPiece.piece, 0, 3, height + 2, false);
    } else {
        heldPiece = lastMove.placedPiece.piece;
        currentPiece.reset(lastMove.heldPiece, 0, 3, height + 2, false);
    }
    held = false;
    currentMove.reset();
    moveHistory.pop();
}

void Board::movePiece(PieceState& currentPiece, int movement) {
    int sign = (movement > 0) - (movement < 0);
    for (int i = 0; i < sign * movement; i++) {
        currentPiece.x += sign;
        if (!validPosition(currentPiece)) {
            currentPiece.x -= sign;
        }
    }
    if (validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, currentPiece.y - 1})) currentPiece.grounded = false;
}

void Board::updatePiecePosition(PieceState& currentPiece, InputState* input, std::chrono::time_point<std::chrono::high_resolution_clock> time) {
    if (!input->isPressed) {
        input->dasTriggered = false;
        return;
    }
    if (time - input->startTime < input->DAS_DELAY) return;

    input->dasTriggered = true;
    if (input->ARR_INTERVAL == std::chrono::milliseconds::zero()) {
        movePiece(currentPiece, input->movement * 100);
        input->lastRepeat = time;
    } else if (time - input->lastRepeat >= input->ARR_INTERVAL) {
        movePiece(currentPiece, input->movement);
        input->lastRepeat = time;
    }
}

void Board::rotatePiece(PieceState& currentPiece, int rotation) {
    PieceState rotated = currentPiece;
    rotated.rotation = (rotated.rotation + rotation + 4) % 4;
    std::vector<std::vector<int>> kicks = getOffsets(rotated, currentPiece.rotation);
    for (std::vector<int> offset : kicks) {
        PieceState temp = rotated;
        temp.x += offset[0];
        temp.y -= offset[1];
        if (validPosition(temp)) {
            currentPiece = temp;
            return;
        }
    }
}

void Board::holdPiece(PieceState& currentPiece) {
    if (held) return;

    currentMove.held = true;
    if (heldPiece != -1) {
        currentMove.heldPiece = currentPiece.piece;
        currentPiece.piece = heldPiece;
        heldPiece = currentMove.heldPiece;
    } else {
        currentMove.heldPiece = -1;
        heldPiece = currentPiece.piece;
        currentPiece.piece = getNextPiece();
    }
    currentPiece.reset(currentPiece.piece, 0, 3, height + 2, false);
    held = true;
}

std::vector<std::vector<int>> Board::getGuideBlock() {
    return guide.getGuideBlocks(piecesPlaced);
}
