#ifndef SRS_HPP
#define SRS_HPP

#include "piece/PieceState.hpp"
#include <vector>

int getRotationIndex(int prevRotation, int nextRotation);
std::vector<std::vector<int>> getOffsets(PieceState currentPiece, int lastRotation);

#endif
