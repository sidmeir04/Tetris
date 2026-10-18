#ifndef PIECE_BAG_HPP
#define PIECE_BAG_HPP

#include <map>
#include <optional>
#include <vector>

struct Constraint {
    int piece;
    int after = -1;
    int pos = -1;
};

std::optional<std::vector<int>> generateRandomConstrainedBag(int n, const std::vector<Constraint>& constraints);

#endif
