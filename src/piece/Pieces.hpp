#ifndef PIECES_HPP
#define PIECES_HPP

#include "piece/PieceState.hpp"
#include <vector>

struct Coordinate {
    int x;
    int y;

    Coordinate(int x_pos, int y_pos) :
        x(x_pos), y(y_pos) {}

    bool operator==(const Coordinate& other) const {
        return x == other.x && 
               y == other.y;
    }

    bool operator!=(const Coordinate& other) const {
        return !(*this==other);
    }
};

const std::vector<std::vector<int>>& getPiece(const PieceState& currentPiece);
const std::vector<std::vector<int>>& getPiece(const Position& position);
const std::vector<Coordinate>& getPieceOffsets(const PieceState& currentPiece);
const std::vector<Coordinate>& getPieceOffsets(const Position& position);

#endif
