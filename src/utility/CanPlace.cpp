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
    board.currentPiece = originalPieceState;
    return possiblePlacements;
}

bool inGuide(const std::vector<std::vector<int>>& guideBlocks, Board* board, std::vector<std::vector<Coordinate>>* seen) {
    if (guideBlocks.empty() || guideBlocks[0].empty()) return false;

    const int guideHeight = static_cast<int>(guideBlocks.size());
    const int guideWidth = static_cast<int>(guideBlocks[0].size());
    const std::vector<std::vector<int>>& piece = getPiece(board->currentPiece);
    const std::vector<Coordinate>& pieceOffsets = getPieceOffsets(board->currentPiece);

    std::vector<Coordinate> pieceBlocks;
    pieceBlocks.reserve(pieceOffsets.size());
    for (const Coordinate& offset : pieceOffsets) {
        int gy = board->currentPiece.y - offset.y;
        int gx = board->currentPiece.x + offset.x;
        if (gy < 0 || gy >= guideHeight || gx < 0 || gx >= guideWidth) {
            return false;
        }
        if (guideBlocks[gy][gx] != piece[offset.y][offset.x]) {
            return false;
        }
        pieceBlocks.emplace_back(gx, gy);
    }

    for (const auto& previous : *seen) {
        bool seen = true;
        for (size_t i = 0; i < previous.size(); ++i) {
            if (previous[i] != pieceBlocks[i]) {
                seen = false;
                break;
            }
        }
        if(seen) return false;
    }

    seen->push_back(pieceBlocks);

    return true;
}

std::vector<Position> getTargetPositions(const std::vector<std::vector<int>>& guideBlocks, Board* board) {
    std::vector<Position> possiblePlacements;
    VisitedTracker visited(board->width, board->height);
    visited.markVisited(board->currentPiece.getPosition());

    std::vector<Position> reachable;
    const size_t maxReachableStates = 4 * static_cast<size_t>(board->width + 4) * static_cast<size_t>(board->height + 6);
    reachable.reserve(maxReachableStates);
    reachable.push_back(board->currentPiece.getPosition());

    std::vector<std::vector<Coordinate>> piecePositions;

    PieceState originalPieceState = board->currentPiece;
    while(!reachable.empty()) {
        Position current = reachable.back();
        reachable.pop_back();
        for(int i = 0; i < 6; i++){
            board->currentPiece.updatePosition(current);
            switch (i) {
                case 0:
                    if(!board->fastRotatePiece(board->currentPiece, -1)) continue;
                    break;
                case 1:
                    if(!board->fastRotatePiece(board->currentPiece, 1)) continue;
                    break;
                case 2:
                    if(!board->fastRotatePiece(board->currentPiece, 2)) continue;
                    break;
                case 3:
                    if(!board->fastMovePiece(board->currentPiece, 1)) continue;;
                    break;
                case 4:
                    if(!board->fastMovePiece(board->currentPiece, -1)) continue;
                    break;
                case 5:
                    if(board->validPosition(board->currentPiece, -1)){
                        board->currentPiece.y--;
                    } else {
                        continue;
                    }
                    break;
            }
            if(!visited.hasBeenReached(board->currentPiece.getPosition())) {
                reachable.push_back(board->currentPiece.getPosition());
                visited.markVisited(board->currentPiece.getPosition());
                if(!board->validPosition(board->currentPiece, -1)) {
                    if(inGuide(guideBlocks, board, &piecePositions)) {
                        possiblePlacements.push_back(board->currentPiece.getPosition());
                    }
                }
            }
        }
    }

    board->currentPiece = originalPieceState;
    return possiblePlacements;
}