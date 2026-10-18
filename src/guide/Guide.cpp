#include "guide/Guide.hpp"
#include "guide/GuidePatterns.hpp"

#include <algorithm>
#include <utility>

Guide::Guide(std::vector<std::vector<int>> blocks, std::string name)
    : guideBlocks(blocks),
      guideName(std::move(name)) {

    refrenceRanges.set(std::make_pair(
        boost::icl::discrete_interval<int>::closed(0, 6),
        new_DT_cannon1
    ));

    refrenceRanges.set(std::make_pair(
        boost::icl::discrete_interval<int>::closed(7, 13),
        new_DT_cannon2
    ));

    refrenceRanges.set(std::make_pair(
        boost::icl::discrete_interval<int>::closed(14, 20),
        new_DT_cannon3
    ));

    // refrenceRanges.set(std::make_pair(
    //     boost::icl::discrete_interval<int>::closed(0, 6),
    //     SDPCguide1
    // ));

    // refrenceRanges.set(std::make_pair(
    //     boost::icl::discrete_interval<int>::closed(7, 13),
    //     SDPCguide2
    // ));

    ranges = refrenceRanges;
}

void Guide::reset() {
    ranges = refrenceRanges;
}

void Guide::clearLines(const std::vector<int>& clearedLines, int piecesPlaced) {
    if (guideBlocks.empty()) {
        return;
    }

    std::vector<int> sortedLines = clearedLines;
    std::sort(sortedLines.begin(), sortedLines.end(), std::greater<int>());

    for (int line : sortedLines) {
        removeRowFromGuide(piecesPlaced, line);
    }
}

bool Guide::removeRowFromGuide(int piecesPlaced, int rowIndex) {
    auto it = ranges.find(piecesPlaced);
    if (it == ranges.end()) {
        return false;
    }

    auto interval = it->first;
    std::vector<std::vector<int>> guideBlocks = it->second;

    if (rowIndex >= guideBlocks.size()) {
        return false;
    }
    guideBlocks.erase(guideBlocks.begin() + rowIndex);

    ranges.set(std::make_pair(interval, guideBlocks));
    return true;
}

std::vector<std::vector<int>> Guide::getGuideBlocks(int piecesPlaced) {
    if (!active) return {{}};
    auto it = ranges.find(piecesPlaced);
    if (it != ranges.end()) {
        return it->second;
    }
    return {{}};
}
