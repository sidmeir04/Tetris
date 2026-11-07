#ifndef PIECES_HPP
#define PIECES_HPP

#include "piece/PieceState.hpp"
#include <vector>

std::vector<std::vector<int>> getPiece(const PieceState& currentPiece);
std::vector<std::vector<int>> getPiece(const Position& position);

#endif
