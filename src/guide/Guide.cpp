#include "guide/Guide.hpp"
#include "guide/GuidePatterns.hpp"

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

void Guide::placeBlock() {
    totalPlaced++;
    placedThisStep++;
    if(placedThisStep >= currentStep->length) {
        if (currentStep->nextSteps[0] == &end && currentStep != &end) {
            end.previousStep = currentStep;
        }
        placedThisStep = 0;
        currentStep = currentStep->nextSteps[0];
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

// Guide::Guide(std::vector<std::vector<int>> blocks, std::string name)
//     : guideBlocks(blocks),
//       guideName(std::move(name)) {

//     refrenceRanges.set(std::make_pair(
//         boost::icl::discrete_interval<int>::closed(0, 6),
//         new_DT_cannon1
//     ));

//     refrenceRanges.set(std::make_pair(
//         boost::icl::discrete_interval<int>::closed(7, 13),
//         new_DT_cannon2
//     ));

//     refrenceRanges.set(std::make_pair(
//         boost::icl::discrete_interval<int>::closed(14, 20),
//         new_DT_cannon3
//     ));

//     // refrenceRanges.set(std::make_pair(
//     //     boost::icl::discrete_interval<int>::closed(0, 6),
//     //     SDPCguide1
//     // ));

//     // refrenceRanges.set(std::make_pair(
//     //     boost::icl::discrete_interval<int>::closed(7, 13),
//     //     SDPCguide2
//     // ));

//     ranges = refrenceRanges;
// }

// void Guide::reset() {
//     ranges = refrenceRanges;
// }

// void Guide::clearLines(const std::vector<int>& clearedLines, int piecesPlaced) {
//     if (guideBlocks.empty()) {
//         return;
//     }

//     std::vector<int> sortedLines = clearedLines;
//     std::sort(sortedLines.begin(), sortedLines.end(), std::greater<int>());

//     for (int line : sortedLines) {
//         removeRowFromGuide(piecesPlaced, line);
//     }
// }

// bool Guide::removeRowFromGuide(int piecesPlaced, int rowIndex) {
//     auto it = ranges.find(piecesPlaced);
//     if (it == ranges.end()) {
//         return false;
//     }

//     auto interval = it->first;
//     std::vector<std::vector<int>> guideBlocks = it->second;

//     if (rowIndex >= guideBlocks.size()) {
//         return false;
//     }
//     guideBlocks.erase(guideBlocks.begin() + rowIndex);

//     ranges.set(std::make_pair(interval, guideBlocks));
//     return true;
// }

// std::vector<std::vector<int>> Guide::getGuideBlocks(int piecesPlaced) {
//     if (!active) return {{}};
//     auto it = ranges.find(piecesPlaced);
//     if (it != ranges.end()) {
//         return it->second;
//     }
//     return {{}};
// }
