#include "guide/Guide.hpp"
#include "guide/GuidePatterns.hpp"
#include "board/Board.hpp"
#include "utility/CanPlace.hpp"

#include <algorithm>
#include <utility>

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
            if (possibleStep(step, board)) {
                currentStep = step;
                return;
            }
        }
        currentStep = currentStep->nextSteps[0]; // Default to the first step if non are possible
    }
}

void Guide::undoBlock() {
    totalPlaced--;
    placedThisStep--;
    if(placedThisStep < 0) {
        currentStep = currentStep->previousStep;
        placedThisStep = currentStep->length - 1;
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
    std::vector<std::vector<int>> guideBlocks = board->guide.getGuideBlocks();
    for (int r = 0; r < 4; r++) {
        std::vector<std::vector<int>> piece = getPiece(Position(targetPiece, r, 0, 0));
        std::vector<Coordinate> toCheck;
        for (int i = 0; i < piece.size(); i++) {
            for (int j = 0; j < piece[i].size(); j++) {
                if (piece[i][j] == targetPiece) {
                    toCheck.push_back({j, i});
                }
            }
        }
        for (int y = 0; y < board->height + 5; y++) {
            for (int x = -3; x < board->width; x++) {
                bool possible = true;
                for (Coordinate pos : toCheck) {
                    if (y-pos.y < 0 || y-pos.y > guideBlocks.size() || x+pos.x < 0 || x+pos.x > board->width || guideBlocks[y-pos.y][x+pos.x] != targetPiece) { // If not working might need to flip -pos.y to +pos.y
                        possible = false;
                        break;
                    }
                }
                if (possible) targetPositions.push_back({targetPiece, r, x, y});
            }
        }
    }
    
    return targetPositions;
}

struct SearchStep {
    int depth;
    std::vector<Position> targetPositions;
    std::vector<Position> possiblePlacements;

    SearchStep* previousStep;
    SearchStep* nextStep;

    SearchStep(int d, SearchStep* previous, std::vector<Position> targets, std::vector<Position> placements)
    : depth(d), previousStep(previous), targetPositions(targets), possiblePlacements(placements) {}
};

bool Guide::possibleStep(GuideStep* step, Board* board) {
    bool possible = false;
    std::vector<Position> targetPositions = getTargetPositions(step, board);
    std::vector<Position> possiblePlacements = getPossiblePlacements(*board, board->currentPiece);
    SearchStep initialStep(0, nullptr, targetPositions, possiblePlacements);
    SearchStep* currentStep = &initialStep;
    while(currentStep->previousStep != nullptr && !currentStep->targetPositions.empty()) {
        if (currentStep->depth == step->length) {
            possible = true;
            break;
        }
        Position target = currentStep->targetPositions.back();
        bool found = false;
        for (Position pos : currentStep->possiblePlacements) {
            if (target == pos) {
                found = true;
                break;
            }
        }
        if (found) {
            board->currentPiece.updatePosition(target);
            board->placePiece();
            currentStep->nextStep = new SearchStep(currentStep->depth+1, currentStep, getTargetPositions(step, board), getPossiblePlacements(*board, board->currentPiece));
            continue;
        }
        currentStep->targetPositions.pop_back();
        if (currentStep->targetPositions.empty()) {
            board->undoPiece();
            currentStep = currentStep->previousStep;
            free(currentStep->nextStep);
            currentStep->nextStep = nullptr;
        }
    }
    return possible;
}