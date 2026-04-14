#include <vector>
#include <deque>
#include <random>
#include <algorithm>
#include <chrono>
#include "pieces.h"
#include "pieceState.h"
#include "srsTable.h"
#include "pieceHandling.h"

struct Board {
    int width;
    int height;
    std::vector<std::vector<int>> grid;

    std::vector<int> pieceBag;
    std::mt19937 gen;
    std::deque<int> pieceQueue;
    PieceState currentPiece;
    int holdPiece = -1;
    bool held = false;

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

    Board(int w, int h)
        : width(w),
          height(h),
          grid(h + 5, std::vector<int>(w, 0)),
          gen(static_cast<unsigned int>(
              std::chrono::system_clock::now().time_since_epoch().count())),
          currentPiece(0, 0, 0, 0, false) {
        currentPiece = PieceState(getNextPiece(), 0, 3, height - 1, false);
        for (int i = 0; i < 6; i++) {
            pieceQueue.push_back(getNextPiece());
        }
    }

    void reset(int w, int h){
        width = w;
        height = h;
        grid.clear();
        grid.assign(h+5, std::vector<int>(w, 0));
    }

    void resetGame(int w, int h) {
        reset(w, h);
        pieceQueue.clear();
        pieceBag.clear();
        for (int i = 0; i < 6; i++) {
            pieceQueue.push_back(getNextPiece());
        }
        currentPiece.reset(currentPiece.piece, 0, 3, height + 1, false);
        currentPiece.piece = getNextPiece();
        holdPiece = -1;
    }

    bool validPosition(const PieceState& state){
        std::vector<std::vector<int>> piece = getPiece(state);
        int x;
        int gridRow;
        for(int i = 0; i < piece.size(); i++){
            for(int j = 0; j < piece[0].size(); j++){
                if(piece[i][j]){
                    x = state.x + j;
                    gridRow = state.y - i;
                    if((x < 0) || (gridRow < 0)){
                        return false;
                    }
                    if(x >= width){
                        return false;
                    }
                    if(grid[gridRow][x]){
                        return false;
                    }
                }
            }
        }
        return true;
    }

    int clearLines() {
        int linesCleared = 0;

        for (int i = height+4; i >= 0; i--) {
            bool isFull = true;

            for (int j = 0; j < width; j++) {
                if (grid[i][j] == 0) {
                    isFull = false;
                    break;
                }
            }

            if (isFull) {
                grid.erase(grid.begin() + i);
                grid.insert(grid.begin() + (height + 4), std::vector<int>(width, 0));
                i++; 
                linesCleared++;
            }
        }
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
                    gridRow = state.y - i;  // y=0 is bottom; state.y is piece top
                    if(gridRow >= 0){
                        grid[gridRow][x] = piece[i][j];
                    }
                }
            }
        }
        currentPiece.reset(getNextPiece(), 0, 3, height+2, false);
        held = false;
        clearLines();
        if (!validPosition(currentPiece)) {
            resetGame(width, height);
            return false;
        }
        return true;
    }

    PieceState movePiece(PieceState currentPiece, int movement){
        int sign = (movement > 0) - (movement < 0);
        for(int i = 0; i < sign*movement; i ++){
            currentPiece.x += sign;
            if(!validPosition(currentPiece)){
                currentPiece.x -= sign;
                return currentPiece;
            } else if(validPosition({currentPiece.piece, currentPiece.rotation, currentPiece.x, currentPiece.y - 1})){
                currentPiece.grounded = false;
            }
        }
        return currentPiece;
    }

    PieceState updatePiecePosition(PieceState currentPiece, InputState* input, std::chrono::time_point<std::chrono::high_resolution_clock> time){
        const std::chrono::milliseconds ZERO(0);
        if (input->isPressed) {
            auto elapsed = time - input->startTime;
            
            if (elapsed >= input->DAS_DELAY) {
                input->dasTriggered = true;
                if(input->ARR_INTERVAL == ZERO){
                    return movePiece(currentPiece, input->movement*100);
                    input->lastRepeat = time;
                }
                auto timeSinceLastRepeat = time - input->lastRepeat;
                if (timeSinceLastRepeat >= input->ARR_INTERVAL) {
                    return movePiece(currentPiece, input->movement);
                    input->lastRepeat = time;
                }
            }
        } else {
            input->dasTriggered = false;
        }
        
        return currentPiece;
    }

    PieceState rotatePiece(PieceState currentPiece, int rotation){
        PieceState rotated = currentPiece;
        rotated.rotation = (rotated.rotation + rotation + 4) % 4;
        std::vector<std::vector<int>> kicks = getOffests(rotated, currentPiece.rotation);
        for(std::vector<int> offset : kicks){
            PieceState temp = rotated;
            temp.x += offset[0];
            temp.y -= offset[1];
            if(validPosition(temp)) return temp;
        }
        return currentPiece;
    }

};
