#include "guide/Guide.hpp"
#include "guide/GuidePatterns.hpp"
#include "board/Board.hpp"
#include "utility/CanPlace.hpp"
#include "utility/Timing.hpp"

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
    if(placedThisStep >= currentStep->length && advanceStep) {
        std::cout << "Changing guide" << std::endl;
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
std::vector<Position> getHoldTargetPositions(const std::vector<std::vector<int>>& guideBlocks, Board* board) {
    if (board->held) {
        return {};
    }

    PieceState originalCurrentPiece = board->currentPiece;
    int originalHeldPiece = board->heldPiece;
    std::deque<int> originalQueue = board->pieceQueue;
    BoardSnapShot originalMove = board->currentMove;
    bool originalHeld = board->held;

    board->holdPiece(board->currentPiece);
    std::vector<Position> targets = getTargetPositions(guideBlocks, board);

    board->currentPiece = originalCurrentPiece;
    board->heldPiece = originalHeldPiece;
    board->pieceQueue = originalQueue;
    board->currentMove = originalMove;
    board->held = originalHeld;
    return targets;
}

std::vector<std::vector<int>> getGuideBlocksAfterPlacement(const std::vector<std::vector<int>>& guideBlocks, Board* board) {
    std::vector<std::vector<int>> updatedGuideBlocks = guideBlocks;
    const std::vector<int>& clearedLines = board->moveHistory.top().lineIndexes;
    for (int line : clearedLines) {
        if (line >= 0 && static_cast<size_t>(line) < updatedGuideBlocks.size()) {
            updatedGuideBlocks.erase(updatedGuideBlocks.begin() + line);
        }
    }
    return updatedGuideBlocks;
}

struct SearchStep {
    int depth;
    bool held = false;
    SearchStep* previousStep;
    std::unique_ptr<SearchStep> nextStep;
    std::vector<std::vector<int>> guideBlocks;
    std::vector<Position> targetPositions;
    std::vector<Position> holdTargetPositions;

    SearchStep(int d, SearchStep* previous, std::vector<std::vector<int>> blocks,
               std::vector<Position> targets, std::vector<Position> holdTargets)
        : depth(d), previousStep(previous), nextStep(nullptr),
          guideBlocks(std::move(blocks)),
          targetPositions(std::move(targets)), 
          holdTargetPositions(std::move(holdTargets)) {}
};

bool Guide::possibleStep(GuideStep* step, Board* board) {
    Timer timer = Timer();

    this->advanceStep = false;
    std::vector<std::vector<int>> guideBlocks = step->guideBlocks;
    std::vector<Position> targetPositions = getTargetPositions(guideBlocks, board);
    std::vector<Position> holdTargetPositions = getHoldTargetPositions(guideBlocks, board);

    std::unique_ptr<SearchStep> initialStep = std::make_unique<SearchStep>(
        0, nullptr, std::move(guideBlocks), std::move(targetPositions), std::move(holdTargetPositions)
    );
    SearchStep* currentStep = initialStep.get();
    while(currentStep->previousStep != nullptr || !currentStep->targetPositions.empty() || !currentStep->holdTargetPositions.empty()) {
        if (currentStep->depth == step->length) {
            std::cout << "Returning true" << std::endl;
            for(int i = 0; i < currentStep->depth; i++) board->undoPiece(); // Undo all the placements
            this->advanceStep = true;
            timer.printTotalElapsedTime();
            return true;
        }

        if (currentStep->targetPositions.empty() && currentStep->holdTargetPositions.empty()) {
            if (currentStep->previousStep != nullptr) {
                board->undoPiece();
                currentStep = currentStep->previousStep;
                currentStep->nextStep.reset();
                continue;
            } else {
                break;
            }
        }

        std::cout << "Looping ";
        timer.printElapsedTime();
        
        if (!currentStep->targetPositions.empty()) {
            Position target = currentStep->targetPositions.back();
            currentStep->targetPositions.pop_back();

            board->currentPiece.updatePosition(target);
            board->placePiece(board->currentPiece);
            std::vector<std::vector<int>> nextGuideBlocks = getGuideBlocksAfterPlacement(currentStep->guideBlocks, board);

            std::unique_ptr<SearchStep> next = std::make_unique<SearchStep>(
                currentStep->depth + 1,
                currentStep,
                nextGuideBlocks,
                getTargetPositions(nextGuideBlocks, board),
                getHoldTargetPositions(nextGuideBlocks, board)
            );
            currentStep->nextStep = std::move(next);
            currentStep = currentStep->nextStep.get();
        } else if (!currentStep->holdTargetPositions.empty()) {
            Position target = currentStep->holdTargetPositions.back();
            currentStep->holdTargetPositions.pop_back();

            board->holdPiece(board->currentPiece);
            board->currentPiece.updatePosition(target);
            board->placePiece(board->currentPiece);
            std::vector<std::vector<int>> nextGuideBlocks = getGuideBlocksAfterPlacement(currentStep->guideBlocks, board);
            
            std::unique_ptr<SearchStep> next = std::make_unique<SearchStep>(
                currentStep->depth + 1,
                currentStep,
                nextGuideBlocks,
                getTargetPositions(nextGuideBlocks, board),
                getHoldTargetPositions(nextGuideBlocks, board)
            );
            currentStep->nextStep = std::move(next);
            currentStep = currentStep->nextStep.get();
        }
    }
    std::cout << "Returning false" << std::endl;
    this->advanceStep = true;
    timer.printTotalElapsedTime();
    return false;
}