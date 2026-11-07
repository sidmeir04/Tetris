#ifndef GUIDE_HPP
#define GUIDE_HPP

#include <climits>
#include <string>
#include <vector>

struct Board; // Forward Declaring

struct GuideStep {
    int length;
    std::vector<std::vector<int>> guideBlocks;
    std::vector<int> clearedLines;
    std::vector<int> clearedTimeStamps;

    GuideStep* previousStep;
    std::vector<GuideStep*> nextSteps;

    GuideStep(int len, GuideStep* previous, std::vector<std::vector<int>> blocks);
};

struct Guide {
    std::string guideName;
    bool active = false;
    GuideStep* initialStep;
    GuideStep* currentStep;

    inline static GuideStep end{INT_MAX, nullptr, {}};

    int totalPlaced = 0;
    int placedThisStep = 0;

    Guide(std::string name);
    void reset();
    void placeBlock(Board* board);
    void undoBlock();
    void clearLines(const std::vector<int>& clearedLines);
    std::vector<std::vector<int>> getGuideBlocks();
    bool possibleStep(GuideStep* step, Board* board);
};

#endif
