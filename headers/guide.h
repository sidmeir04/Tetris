#ifndef GUIDE
#define GUIDE

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include <boost/icl/interval_map.hpp>

std::vector<std::vector<int>> SDPCguide1 = {
    {4,4,3,3,0,0,0,6,2,5},
    {4,4,7,3,3,6,6,6,2,5},
    {1,7,7,7,0,0,0,2,2,5},
    {1,1,0,0,0,0,0,0,0,5},
    {0,1,0,0,0,0,0,0,0,0}
};

std::vector<std::vector<int>> SDPCguide2 = {
    {0,0,0,0,3,3,0,0,0,0},
    {0,0,0,0,0,3,3,0,0,0},
    {0,0,1,0,0,0,2,2,2,0},
    {5,0,1,1,0,0,0,0,2,6},
    {5,4,4,1,0,0,0,6,6,6},
    {5,4,4,0,0,0,0,0,0,0},
    {5,0,0,0,0,0,0,0,0,0}
};

struct Guide
{
    std::vector<std::vector<int>> guideBlocks;
    std::string guideName;
    bool active = true;

    boost::icl::interval_map<int, std::vector<std::vector<int>>> refrenceRanges;
    boost::icl::interval_map<int, std::vector<std::vector<int>>> ranges;

    Guide(std::vector<std::vector<int>> blocks, std::string name)
        : guideBlocks(blocks),
          guideName(std::move(name)) {
        refrenceRanges.set(std::make_pair(
            boost::icl::discrete_interval<int>::closed(0, 6),
            SDPCguide1
        ));

        refrenceRanges.set(std::make_pair(
            boost::icl::discrete_interval<int>::closed(7, 13),
            SDPCguide2
        ));

        ranges = refrenceRanges;
    }

    void reset(){
        ranges = refrenceRanges;
    }

    void clearLines(const std::vector<int>& clearedLines, int piecesPlaced) {
        if (guideBlocks.empty()) {
            return;
        }

        std::vector<int> sortedLines = clearedLines;
        std::sort(sortedLines.begin(), sortedLines.end(), std::greater<int>());

        for (int line : sortedLines) {
            removeRowFromGuide(piecesPlaced, line);
        }
    }

    bool removeRowFromGuide(int piecesPlaced, int rowIndex) {
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

    std::vector<std::vector<int>> getGuideBlocks(int piecesPlaced) {
        if(!active) return {{}};
        auto it = ranges.find(piecesPlaced);
        if (it != ranges.end()) {
            return it->second;
        }
        return {{}};
    }
};

#endif