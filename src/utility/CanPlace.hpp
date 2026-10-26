#ifndef CAN_PLACE_HPP
#define CAN_PLACE_HPP

#include "board/Board.hpp"
#include "piece/PieceState.hpp"
#include "piece/Pieces.hpp"

#include <string>
#include <vector>

struct Position {
    int piece;
    int rotation;
    int x;
    int y;
    
    Position(int p = 0, int r = 0, int x_pos = 0, int y_pos = 0)
     : piece(p), rotation(r), x(x_pos), y(y_pos) {}
}

std::vector<Position> getPossiblePlacements(Board& board, PieceState& piece);

#endif