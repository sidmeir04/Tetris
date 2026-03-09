#ifndef PIECE_STATE
#define PIECE_STATE

struct PieceState {
    int piece;
    int rotation;
    int x;
    int y;
    
    PieceState(int p = 0, int r = 0, int x_pos = 0, int y_pos = 0)
        : piece(p), rotation(r), x(x_pos), y(y_pos) {}
};

#endif