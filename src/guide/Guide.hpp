#ifndef GUIDE_HPP
#define GUIDE_HPP

#include <climits>
#include <string>
#include <vector>

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
    void placeBlock();
    void undoBlock();
    void clearLines(const std::vector<int>& clearedLines);
    std::vector<std::vector<int>> getGuideBlocks();
};

// struct Guide {
//     std::vector<std::vector<int>> guideBlocks;
//     std::string guideName;
//     bool active = true;

//     boost::icl::interval_map<int, std::vector<std::vector<int>>> refrenceRanges;
//     boost::icl::interval_map<int, std::vector<std::vector<int>>> ranges;

//     Guide(std::vector<std::vector<int>> blocks, std::string name);

//     void reset();
//     void clearLines(const std::vector<int>& clearedLines, int piecesPlaced);
//     bool removeRowFromGuide(int piecesPlaced, int rowIndex);
//     std::vector<std::vector<int>> getGuideBlocks(int piecesPlaced);
// };

#endif
