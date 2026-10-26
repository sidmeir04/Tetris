#include "board/Board.hpp"
#include "piece/PieceState.hpp"
#include "piece/Pieces.hpp"

#include <string>
#include <vector>

std::vector<Position> getPossiblePlacements(Board& board, PieceState& piece) {
    std::vector<Position> possiblePlacements;

    for(int i = 0; i < board.height + 5; i++){
        for(int j = 0; j < board.width; j++){
            for(int r  = 0; r < 4; r++){
                // Loop through allk possible possitions and rotations
                if(!board.validPosition(piece)) continue; // Invalid position
                if(board.validPosition(piece, -1)) continue; // Floating in mid air

                if(i<=piece.y){ // Check if can move and hard drop
                    PieceState temp = piece;
                    while (temp.rotation != r) {
                        temp.rotation++;
                        if(!board.validPosition(temp)) continue;
                    }
                    if(piece.x < j){
                        for(int x = piece.x; piece.x < j; temp.x++) {
                            if(!board.validPosition(temp)) continue;
                        }
                    }
                    else {
                        for(int x = piece.x; piece.x < j; temp.x--) {
                            if(!board.validPosition(temp)) continue;
                        }
                    }
                    for(int y_off; j+y_off > piece.y; y_off--) {
                        if (!board.validPosition(temp, y_off)) continue;
                    }
                }


            }
        }
    }

    return possiblePlacements;
}