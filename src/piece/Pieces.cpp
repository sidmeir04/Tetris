#include "piece/Pieces.hpp"

#include <stdexcept>
#include <string>

// I = 5
// Square = 4
// T = 7
// L = 2
// J = 6
// S = 1
// Z = 3
namespace {
std::vector<std::vector<std::vector<std::vector<int>>>> pieces = {
    //Line
    {
        //Roation 1
        {
            {0,0,0,0},
            {5,5,5,5},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 2
        {
            {0,0,5,0},
            {0,0,5,0},
            {0,0,5,0},
            {0,0,5,0}
        },
        //Roation 3
        {
            {0,0,0,0},
            {0,0,0,0},
            {5,5,5,5},
            {0,0,0,0}
        },
        //Roation 4
        {
            {0,5,0,0},
            {0,5,0,0},
            {0,5,0,0},
            {0,5,0,0}
        }
    },
    //Square
    {
        //Roation 1
        {
            {0,4,4,0},
            {0,4,4,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 2
        {
            {0,4,4,0},
            {0,4,4,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 3
        {
            {0,4,4,0},
            {0,4,4,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 4
        {
            {0,4,4,0},
            {0,4,4,0},
            {0,0,0,0},
            {0,0,0,0}
        }
    },
    //T-Piece
    {
        //Roation 1
        {
            {0,7,0,0},
            {7,7,7,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 2
        {
            {0,7,0,0},
            {0,7,7,0},
            {0,7,0,0},
            {0,0,0,0}
        },
        //Roation 3
        {
            {0,0,0,0},
            {7,7,7,0},
            {0,7,0,0},
            {0,0,0,0}
        },
        //Roation 4
        {
            {0,7,0,0},
            {7,7,0,0},
            {0,7,0,0},
            {0,0,0,0}
        }
    },
    //L-Piece
    {
        //Roation 1
        {
            {0,0,2,0},
            {2,2,2,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 2
        {
            {0,2,0,0},
            {0,2,0,0},
            {0,2,2,0},
            {0,0,0,0}
        },
        //Roation 3
        {
            {0,0,0,0},
            {2,2,2,0},
            {2,0,0,0},
            {0,0,0,0}
        },
        //Roation 4
        {
            {2,2,0,0},
            {0,2,0,0},
            {0,2,0,0},
            {0,0,0,0}
        }
    },
    //J-Piece
    {
        //Roation 1
        {
            {6,0,0,0},
            {6,6,6,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 2
        {
            {0,6,6,0},
            {0,6,0,0},
            {0,6,0,0},
            {0,0,0,0}
        },
        //Roation 3
        {
            {0,0,0,0},
            {6,6,6,0},
            {0,0,6,0},
            {0,0,0,0}
        },
        //Roation 4
        {
            {0,6,0,0},
            {0,6,0,0},
            {6,6,0,0},
            {0,0,0,0}
        }
    },
    //S-Piece
    {
        //Roation 1
        {
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 2
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        //Roation 3
        {
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        //Roation 4
        {
            {0,1,0,0},
            {1,1,0,0},
            {1,0,0,0},
            {0,0,0,0}
        }
    },
    //Z-Piece
    {
        //Roation 1
        {
            {0,3,3,0},
            {3,3,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        //Roation 2
        {
            {0,3,0,0},
            {0,3,3,0},
            {0,0,3,0},
            {0,0,0,0}
        },
        //Roation 3
        {
            {0,0,0,0},
            {0,3,3,0},
            {3,3,0,0},
            {0,0,0,0}
        },
        //Roation 4
        {
            {3,0,0,0},
            {3,3,0,0},
            {0,3,0,0},
            {0,0,0,0}
        }
    }
};
}

std::vector<std::vector<int>> getPiece(const Position& position) {
    if (position.piece < 0 || position.piece >= static_cast<int>(pieces.size())) {
        throw std::out_of_range("Invalid piece index: " + std::to_string(position.piece));
    }
    if (position.rotation < 0 || position.rotation >= static_cast<int>(pieces[position.piece].size())) {
        throw std::out_of_range("Invalid piece rotation: " + std::to_string(position.rotation));
    }
    return pieces[position.piece][position.rotation];
}

std::vector<std::vector<int>> getPiece(const PieceState& currentPiece) {
    if (currentPiece.piece < 0 || currentPiece.piece >= static_cast<int>(pieces.size())) {
        throw std::out_of_range("Invalid piece index: " + std::to_string(currentPiece.piece));
    }
    if (currentPiece.rotation < 0 || currentPiece.rotation >= static_cast<int>(pieces[currentPiece.piece].size())) {
        throw std::out_of_range("Invalid piece rotation: " + std::to_string(currentPiece.rotation));
    }
    return pieces[currentPiece.piece][currentPiece.rotation];
    // return getPiece(currentPiece.getPosition());
}