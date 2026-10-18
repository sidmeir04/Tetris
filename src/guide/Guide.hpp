#ifndef GUIDE_HPP
#define GUIDE_HPP

#include <boost/icl/interval_map.hpp>
#include <string>
#include <vector>

struct Guide {
    std::vector<std::vector<int>> guideBlocks;
    std::string guideName;
    bool active = true;

    boost::icl::interval_map<int, std::vector<std::vector<int>>> refrenceRanges;
    boost::icl::interval_map<int, std::vector<std::vector<int>>> ranges;

    Guide(std::vector<std::vector<int>> blocks, std::string name);

    void reset();
    void clearLines(const std::vector<int>& clearedLines, int piecesPlaced);
    bool removeRowFromGuide(int piecesPlaced, int rowIndex);
    std::vector<std::vector<int>> getGuideBlocks(int piecesPlaced);
};

#endif
