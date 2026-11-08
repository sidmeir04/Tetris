#include "guide/Guide.hpp"
#include "guide/GuidePatterns.hpp"
#include "board/Board.hpp"
#include "utility/CanPlace.hpp"

#include <algorithm>
#include <utility>
#include <memory>
#include <iostream>

GuideStep::GuideStep(int len, GuideStep* previous, std::vector<std::vector<int>> blocks)
    : length(len), previousStep(previous), guideBlocks(blocks) {
        clearedLines = {};
        clearedTimeStamps = {};
    }

Guide::Guide(std::string name)
    : guideName(name) {
    end.nextSteps.push_back(&end);
    GuideStep* step1 = new GuideStep(7, nullptr, DT_cannon1);
    GuideStep* step2 = new GuideStep(7, step1, DT_cannon2);
    GuideStep* step3 = new GuideStep(7, step2, DT_cannon3);

    step1->nextSteps.push_back(step2);
    step2->nextSteps.push_back(step3);
    step3->nextSteps.push_back(&end);

    currentStep = step1;
    initialStep = step1;
}

void Guide::reset() {
    while(currentStep->previousStep!=nullptr) {
        currentStep->clearedLines.clear();
        currentStep = currentStep->previousStep;
    }
    totalPlaced = 0;
    placedThisStep = 0;
}

void Guide::clearLines(const std::vector<int>& clearedLines) {
    for(int line:clearedLines){
        currentStep->clearedLines.push_back(line);
        currentStep->clearedTimeStamps.push_back(placedThisStep);
    }
}

void Guide::placeBlock(Board* board) {
    totalPlaced++;
    placedThisStep++;
    if(placedThisStep >= currentStep->length) {
        placedThisStep = 0;
        if (currentStep == &end) return;
        if (currentStep->nextSteps[0] == &end || currentStep->nextSteps.empty() || currentStep->nextSteps[0] == nullptr) {
            end.previousStep = currentStep;
            currentStep = &end;
            return;
        }
        for (GuideStep* step : currentStep->nextSteps) {
            std::cout << "Checking Step" << std::endl;
            if (possibleStep(step, board)) {
                currentStep = step;
                return;
            }
        }
        std::cout << "None Found :<" << std::endl;
        currentStep = currentStep->nextSteps[0]; // Default to the first step if non are possible
    }
}

void Guide::undoBlock() {
    if (totalPlaced <= 0) return;
    totalPlaced--;
    placedThisStep--;
    if(placedThisStep < 0) {
        if (currentStep->previousStep != nullptr) {
            currentStep = currentStep->previousStep;
            placedThisStep = currentStep->length - 1;
        } else {
            placedThisStep = 0;
        }
    }
    while (!currentStep->clearedTimeStamps.empty() && 
            currentStep->clearedTimeStamps.back() == placedThisStep) {
        currentStep->clearedTimeStamps.pop_back();
        currentStep->clearedLines.pop_back();
    }
}

std::vector<std::vector<int>> Guide::getGuideBlocks() {
    if(!active) return {{}};

    std::vector<std::vector<int>> final = currentStep->guideBlocks;
    for (int line : currentStep->clearedLines) {
        if (line < 0 || line >= final.size()) continue;
        final.erase(final.begin() + line);
    }
    return final;
}

// I = 5
// Square = 4
// T = 7
// L = 2
// J = 6
// S = 1
// Z = 3
struct Coordinate {
    int x;
    int y;

    Coordinate(int x_pos, int y_pos) :
        x(x_pos), y(y_pos) {}
};

std::vector<Position> getTargetPositions(GuideStep* step, Board* board) {
    std::vector<Position> targetPositions;
    int targetPiece = board->currentPiece.piece;
    const std::vector<std::vector<int>>& guideBlocks = board->guide.getGuideBlocks();
    std::vector<Position> possiblePlacements = getPossiblePlacements(*board, board->currentPiece);

    if (guideBlocks.empty() || guideBlocks[0].empty()) {
        return targetPositions;
    }

    int guideHeight = guideBlocks.size();
    int guideWidth = guideBlocks[0].size();

    for (Position pos : possiblePlacements) {
        std::vector<std::vector<int>> piece = getPiece(Position(targetPiece, pos.rotation, 0, 0));
        std::vector<Coordinate> toCheck;

        for (size_t i = 0; i < piece.size(); i++) {
            for (size_t j = 0; j < piece[i].size(); j++) {
                if (piece[i][j] == targetPiece) {
                    toCheck.emplace_back(j, i);
                }
            }
        }

        bool possible = true;
        for (const Coordinate& pos : toCheck) {
            int gy = pos.y - pos.y;
            int gx = pos.x + pos.x;
            if (gy < 0 || gy >= guideHeight || gx < 0 || gx >= guideWidth || guideBlocks[gy][gx] != targetPiece) {
                possible = false;
                break;
            }
        }
        if (possible) {
            targetPositions.push_back({targetPiece, pos.rotation, pos.x, pos.y});
        }
    }

    // for (int r = 0; r < 4; r++) {
    //     std::vector<std::vector<int>> piece = getPiece(Position(targetPiece, r, 0, 0));
    //     std::vector<Coordinate> toCheck;

    //     for (size_t i = 0; i < piece.size(); i++) {
    //         for (size_t j = 0; j < piece[i].size(); j++) {
    //             if (piece[i][j] == targetPiece) {
    //                 toCheck.emplace_back(j, i);
    //             }
    //         }
    //     }

    //     for (int y = 0; y < board->height + 5; y++) {
    //         for (int x = -3; x < board->width; x++) {
    //             bool possible = true;
    //             for (const Coordinate& pos : toCheck) {
    //                 int gy = y - pos.y;
    //                 int gx = x + pos.x;
    //                 if (gy < 0 || gy >= guideHeight || gx < 0 || gx >= guideWidth || guideBlocks[gy][gx] != targetPiece) {
    //                     possible = false;
    //                     break;
    //                 }
    //             }
    //             if (possible) {
    //                 targetPositions.push_back({targetPiece, r, x, y});
    //             }
    //         }
    //     }
    // }
    for(Position pos : targetPositions) {
        std::cout << "Target position: " << pos.piece << " " << pos.x << " " << pos.y << " " << pos.rotation << std::endl;
    }
    return targetPositions;
}

struct SearchStep {
    int depth;
    std::vector<Position> targetPositions;
    std::vector<Position> possiblePlacements;

    SearchStep* previousStep;
    std::unique_ptr<SearchStep> nextStep;

    SearchStep(int d, SearchStep* previous, std::vector<Position> targets, std::vector<Position> placements)
        : depth(d), previousStep(previous), 
          targetPositions(std::move(targets)), 
          possiblePlacements(std::move(placements)), 
          nextStep(nullptr) {}
};

bool Guide::possibleStep(GuideStep* step, Board* board) {
    board->updateGuide = false;
    std::vector<Position> targetPositions = getTargetPositions(step, board);
    std::vector<Position> possiblePlacements = getPossiblePlacements(*board, board->currentPiece);
    std::unique_ptr<SearchStep> initialStep = std::make_unique<SearchStep>(0, nullptr, std::move(targetPositions), std::move(possiblePlacements));
    SearchStep* currentStep = initialStep.get();
    while(currentStep->previousStep != nullptr || !currentStep->targetPositions.empty()) {
        // std::cout << "Stepping" << std::endl;
        if (currentStep->depth == step->length) {
            std::cout << "Return true" << std::endl;
            board->updateGuide = true;
            return true;
        }
        if (currentStep->targetPositions.empty()) {
            if (currentStep->previousStep != nullptr) {
                std::cout << "Empty "<< std::endl;
                board->undoPiece();
                currentStep = currentStep->previousStep;
                currentStep->nextStep.reset();
                continue;
            } else {
                std::cout << "Breaking"<< std::endl;
                break;
            }
        }
        Position target = currentStep->targetPositions.back();
        currentStep->targetPositions.pop_back();
        bool found = false;
        for (Position pos : currentStep->possiblePlacements) {
            if (target == pos) {
                found = true;
                break;
            }
        }
        if (found) {
            board->currentPiece.updatePosition(target);
            std::cout << "Placing: " << target.x << "," << target.y << "," << board->currentPiece.x << "," << board->currentPiece.y << std::endl;
            board->placePiece(board->currentPiece);
            std::unique_ptr<SearchStep> next = std::make_unique<SearchStep>(
                currentStep->depth + 1,
                currentStep,
                getTargetPositions(step, board),
                getPossiblePlacements(*board, board->currentPiece)
            );
            currentStep->nextStep = std::move(next);
            currentStep = currentStep->nextStep.get();
            continue;
        }
    }
    std::cout << "Return false" << std::endl;
    board->updateGuide = true;
    return false;
}