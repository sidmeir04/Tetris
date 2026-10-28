#ifndef PIECE_STATE_HPP
#define PIECE_STATE_HPP

struct Position {
    int piece;
    int rotation;
    int x;
    int y;
    
    Position(int p = 0, int r = 0, int x_pos = 0, int y_pos = 0)
     : piece(p), rotation(r), x(x_pos), y(y_pos) {}

    bool operator==(const Position& other) const {
        return piece == other.piece && 
               rotation == other.rotation && 
               x == other.x && 
               y == other.y;
    }
};

struct PieceState {
    int piece;
    int rotation;
    int x;
    int y;
    bool grounded;

    PieceState(int p = 0, int r = 0, int x_pos = 0, int y_pos = 0, bool g = false)
        : piece(p), rotation(r), x(x_pos), y(y_pos), grounded(g) {}

    void reset(int p = 0, int r = 0, int x_pos = 0, int y_pos = 0, bool g = false) {
        piece = p;
        rotation = r;
        x = x_pos;
        y = y_pos;
        grounded = g;
    }

    Position getPosition() {
        return Position(piece, rotation, x, y);
    }

    void updatePosition(Position position) {
        piece = position.piece;
        rotation = position.rotation;
        x = position.x;
        y = position.y;
    }
};

#endif
