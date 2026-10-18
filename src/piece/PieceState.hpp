#ifndef PIECE_STATE_HPP
#define PIECE_STATE_HPP

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
};

#endif
