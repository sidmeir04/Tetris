#include <vector>
#include "pieces.h"
#include "pieceState.h"
#include "srsTable.h"

struct Board {
    int width;
    int height;
    std::vector<std::vector<int>> grid;

    Board(int w, int h) : width(w), height(h), grid(h+5, std::vector<int>(w, 0)) {}

    
    bool validPosition(const PieceState& state){
        std::vector<std::vector<int>> piece = getPiece(state);
        int x;
        int gridRow;
        for(int i = 0; i < piece.size(); i++){
            for(int j = 0; j < piece[0].size(); j++){
                if(piece[i][j]){
                    x = state.x + j;
                    gridRow = state.y - i;  // y=0 is bottom; state.y is piece top, i=0 is top of piece
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

        for (int i = height - 1; i >= 0; i--) {
            bool isFull = true;

            for (int j = 0; j < width; j++) {
                if (grid[i][j] == 0) {
                    isFull = false;
                    break;
                }
            }

            if (isFull) {
                // 1. Remove the full row
                grid.erase(grid.begin() + i);

                // 2. Insert a brand new empty row at the very top (high index; y=0 is bottom)
                grid.insert(grid.begin() + (height - 1), std::vector<int>(width, 0));

                // 3. Since we just moved a new row into the current index 'i',
                // we need to check this same index again on the next loop.
                i++; 
                
                linesCleared++;
            }
        }
        return linesCleared;
    }

    void placePiece(const PieceState& state){
        std::vector<std::vector<int>> piece = getPiece(state);
        int x;
        int gridRow;
        for(int i = 0; i < piece.size(); i++){
            for(int j = 0; j < piece[0].size(); j++){
                if(piece[i][j]){
                    x = state.x + j;
                    gridRow = state.y - i;  // y=0 is bottom; state.y is piece top
                    if(gridRow >= 0 && gridRow < height){
                        grid[gridRow][x] = piece[i][j];
                    }
                }
            }
        }
    }

    PieceState rotatePiece(PieceState currentPiece, int rotation){
        PieceState rotated = currentPiece;
        rotated.rotation = (rotated.rotation + rotation + 4) % 4;
        std::vector<std::vector<int>> kicks = getSRS(rotated);
        if(!validPosition(rotated)){
            for(std::vector<int> offset : kicks){
                PieceState temp = rotated;
                temp.x += offset[0];
                temp.y -= offset[1];  // negate: SRS has y+ down, we have y- down (y=0 at bottom)
                if(validPosition(temp)) return temp;
            }
        } else {
            return rotated;
        }
        return currentPiece;
    }

};
