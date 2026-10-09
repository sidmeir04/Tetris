#include <vector>
#include <deque>
#include <stack>
#include <random>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <optional>
#include "pieces.h"
#include "pieceState.h"
#include "srsTable.h"
#include "pieceHandling.h"
#include "guide.h"

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

    // Rendering stuff
    const int tileSize = 32;
    int xBuffer = 200;
    int yBuffer = 80;

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
        if(pieceQueue.size() < 6) {
            pieceQueue.push_back(generateNextPiece());
        }

        int nextPiece = pieceQueue.front();
        pieceQueue.pop_front();

        return nextPiece;
    }

    struct Constraint {
        int piece;
        int after = -1;
        int pos = -1;
    };

    std::optional<std::vector<int>> generateRandomConstrainedBag(int n, const std::vector<Constraint>& constraints) {
        std::map<int, int> fixed_pos_to_val; // Key: Index -> Value: Piece
        std::map<int, int> val_to_fixed_pos; // Key: Piece -> Value: Index (for lookahead)
        std::multimap<int, int> piece_must_be_after; // Key: Piece -> Value: Must be after this piece

        // 1. Process Constraints
        for (const auto& con : constraints) {
            if (con.pos != -1) {
                if (fixed_pos_to_val.count(con.pos) && fixed_pos_to_val[con.pos] != con.piece) return std::nullopt;
                
                fixed_pos_to_val[con.pos] = con.piece;
                val_to_fixed_pos[con.piece] = con.pos;
            }
            if (con.after != -1) {
                piece_must_be_after.insert({con.piece, con.after});
            }
        }

        std::vector<int> result;
        std::random_device rd;
        std::mt19937 g(rd());

        for (int i = 0; i < n; i++) {
            std::vector<int> possible;

            if (fixed_pos_to_val.count(i)) {
                int target = fixed_pos_to_val[i];
                
                auto range = piece_must_be_after.equal_range(target);
                for (auto it = range.first; it != range.second; ++it) {
                    int dependency = it->second;
                    if (std::find(result.begin(), result.end(), dependency) == result.end()) {
                        return std::nullopt; 
                    }
                }
                result.push_back(target);
                continue;
            }

            for (int num = 0; num < n; num++) {
                if (std::find(result.begin(), result.end(), num) != result.end()) continue;
                if (val_to_fixed_pos.count(num) && val_to_fixed_pos[num] != i) continue;

                bool dependencies_met = true;
                auto range = piece_must_be_after.equal_range(num);
                for (auto it = range.first; it != range.second; ++it) {
                    int dependency = it->second;
                    if (std::find(result.begin(), result.end(), dependency) == result.end()) {
                        dependencies_met = false;
                        break;
                    }
                }

                if (dependencies_met) {
                    possible.push_back(num);
                }
            }

            if (possible.empty()) return std::nullopt;

            std::uniform_int_distribution<> dis(0, possible.size() - 1);
            int chosen = possible[dis(g)];
            result.push_back(chosen);
        }

        return result;
    }

    Board(int w, int h)
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

    bool validPosition(const PieceState& state, int yOffset = 0) {
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

    void resetBoard(int w, int h){
        width = w;
        height = h;
        grid.clear();
        grid.assign(h+5, std::vector<int>(w, 0));
        while(!moveHistory.empty()) moveHistory.pop(); //maybe refactor to something else so can use .clear() IDK thou?
    }

    void resetGame(int w, int h) {
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

    std::vector<int> clearLines() {
        std::vector<int> linesCleared = {};
        for (int i = height+4; i >= 0; i--) {
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

    bool placePiece(const PieceState& state){
        std::vector<std::vector<int>> piece = getPiece(state);
        int x;
        int gridRow;
        for(int i = 0; i < piece.size(); i++){
            for(int j = 0; j < piece[0].size(); j++){
                if(piece[i][j]){
                    x = state.x + j;
                    gridRow = state.y - i;
                    if(gridRow >= 0){
                        grid[gridRow][x] = piece[i][j];
                    }
                }
            }
        }
        currentMove.placedPiece = currentPiece;
        clearLines();
        moveHistory.push(currentMove);
        currentMove.reset();
        currentPiece.reset(getNextPiece(), 0, 3, height+2, false);
        held = false;
        if (!validPosition(currentPiece, -1)) currentPiece.grounded = true;
        piecesPlaced++;
        if (!validPosition(currentPiece)) return false;
        return true;
    }

    void undoPiece() {
        if (moveHistory.empty()) return;

        BoardSnapShot lastMove = moveHistory.top();
        if(lastMove.linesCleared){
            for(auto it = lastMove.lineIndexes.rbegin(); it != lastMove.lineIndexes.rend(); ++it){
                grid.insert(grid.begin() + *it, lastMove.lineInformation.back());
                lastMove.lineInformation.pop_back();
            }
        }
        int x;
        int gridRow;
        std::vector<std::vector<int>> piece = getPiece(lastMove.placedPiece);
        for(int i = 0; i < piece.size(); i++){
            for(int j = 0; j < piece[0].size(); j++){
                if(piece[i][j]){
                    x = lastMove.placedPiece.x + j;
                    gridRow = lastMove.placedPiece.y - i;
                    if(gridRow >= 0){
                        grid[gridRow][x] = 0;
                    }
                }
            }
        }
        pieceQueue.push_front(currentPiece.piece);
        if(!lastMove.held){
            currentPiece.reset(lastMove.placedPiece.piece, 0, 3, height+2, false);
            held = false;
        } else if (lastMove.heldPiece == -1) {
            heldPiece = -1;
            currentPiece.reset(lastMove.placedPiece.piece, 0, 3, height+2, false);
        } else {
            heldPiece = lastMove.placedPiece.piece;
            currentPiece.reset(lastMove.heldPiece, 0, 3, height+2, false);
        }
        held = false;
        currentMove.reset();
        moveHistory.pop();
    }

    void movePiece(PieceState& currentPiece, int movement){
        int sign = (movement > 0) - (movement < 0);
        for(int i = 0; i < sign*movement; i ++){
            currentPiece.x += sign;
            if(!validPosition(currentPiece)){
                currentPiece.x -= sign;
            }
        }
        if(validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, currentPiece.y - 1})) currentPiece.grounded = false;
    }

    void updatePiecePosition(PieceState& currentPiece, InputState* input, std::chrono::time_point<std::chrono::high_resolution_clock> time) {
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

    void rotatePiece(PieceState& currentPiece, int rotation){
        PieceState rotated = currentPiece;
        rotated.rotation = (rotated.rotation + rotation + 4) % 4;
        std::vector<std::vector<int>> kicks = getOffests(rotated, currentPiece.rotation);
        for(std::vector<int> offset : kicks){
            PieceState temp = rotated;
            temp.x += offset[0];
            temp.y -= offset[1];
            if(validPosition(temp)) {
                currentPiece = temp;
                return;
            }
        }
    }

    void holdPiece(PieceState& currentPiece) {
        if(held) return;

        currentMove.held = true;
        if(heldPiece != -1){
            currentMove.heldPiece = currentPiece.piece; // Also using the 'temp' variable
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

    std::vector<std::vector<int>> getGuideBlock(){
        return guide.getGuideBlocks(piecesPlaced);
    }

};
