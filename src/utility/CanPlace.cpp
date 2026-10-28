#include "utility/canPlace.hpp"
#include "board/Board.hpp"
#include "piece/PieceState.hpp"
#include "piece/Pieces.hpp"

#include <string>
#include <vector>
#include <deque>
#include <iostream>

bool possiblePlacement(Board& board, PieceState& currentPiece, Position targetPosition) {
    VisitedTracker visited(board.width, board.height);
    std::deque<Position> reachable;
    reachable.push_front(currentPiece.getPosition());
    visited.markVisited(currentPiece.getPosition());

    PieceState originalPieceState = currentPiece;
    while(!reachable.empty()) {
        Position current = reachable.front();
        reachable.pop_front();
        for(int i = 0; i < 6; i++){
            currentPiece.updatePosition(current);
            switch (i) {
                case 0:
                    board.rotatePiece(currentPiece, -1);
                    break;
                case 1:
                    board.rotatePiece(currentPiece, 1);
                    break;
                case 2:
                    board.rotatePiece(currentPiece, 2);
                    break;
                case 3:
                    board.movePiece(currentPiece, 1);
                    break;
                case 4:
                    board.movePiece(currentPiece, -1);
                    break;
                case 5:
                    if(board.validPosition(currentPiece, -1))currentPiece.y--;
                    break;
            }
            if(!visited.hasBeenReached(currentPiece.getPosition())) {
                if(currentPiece.getPosition() == targetPosition) {
                    if(board.validPosition(currentPiece, -1)) return false;
                    else return true;
                }
                reachable.push_front(currentPiece.getPosition());
                visited.markVisited(currentPiece.getPosition());
            }
        }
    }
    return false;
}

std::vector<Position> getPossiblePlacements(Board& board, PieceState& currentPiece) {
    int count = 0;
    std::vector<Position> possiblePlacements;
    VisitedTracker visited(board.width, board.height);
    std::deque<Position> reachable;
    reachable.push_front(currentPiece.getPosition());
    visited.markVisited(currentPiece.getPosition());

    PieceState originalPieceState = currentPiece;
    while(!reachable.empty()) {
        Position current = reachable.front();
        reachable.pop_front();
        for(int i = 0; i < 6; i++){
            count++;
            currentPiece.updatePosition(current);
            switch (i) {
                case 0:
                    board.rotatePiece(currentPiece, -1);
                    break;
                case 1:
                    board.rotatePiece(currentPiece, 1);
                    break;
                case 2:
                    board.rotatePiece(currentPiece, 2);
                    break;
                case 3:
                    board.movePiece(currentPiece, 1);
                    break;
                case 4:
                    board.movePiece(currentPiece, -1);
                    break;
                case 5:
                    if(board.validPosition(currentPiece, -1))currentPiece.y--;
                    break;
            }
            if(!visited.hasBeenReached(currentPiece.getPosition())) {
                reachable.push_front(currentPiece.getPosition());
                visited.markVisited(currentPiece.getPosition());
                if(!board.validPosition(currentPiece, -1)) possiblePlacements.push_back(currentPiece.getPosition());
            }
        }
    }
    std::cout << count;
    return possiblePlacements;
}