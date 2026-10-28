#ifndef CAN_PLACE_HPP
#define CAN_PLACE_HPP

#include "board/Board.hpp"
#include "piece/PieceState.hpp"
#include "piece/Pieces.hpp"

#include <string>
#include <vector>

class VisitedTracker {
    int width;
    int height;
    // Flat 4D storage mapped to 1D: [piece][rotation][x_shifted][y]
    std::vector<bool> visited;

    int getIndex(const Position& p) const {
        int x_shifted = p.x + 3; // Shift range from [-3, width] to [0, width + 3]
        int x_range = width + 4;
        int y_range = height + 6;
        int rot_range = 4;

        // Flattening formula
        return p.piece * (rot_range * x_range * y_range) +
               p.rotation * (x_range * y_range) +
               x_shifted * y_range +
               p.y;
    }

public:
    VisitedTracker(int board_width, int board_height) 
        : width(board_width), height(board_height) {
        int total_states = 7 * 4 * (width + 4) * (height + 6);
        visited.resize(total_states, false);
    }

    bool hasBeenReached(const Position& p) const {
        return visited[getIndex(p)];
    }

    void markVisited(const Position& p) {
        visited[getIndex(p)] = true;
    }

    void clear() {
        std::fill(visited.begin(), visited.end(), false);
    }
};

std::vector<Position> getPossiblePlacements(Board& board, PieceState& piece);

#endif