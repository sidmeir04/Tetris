#include "board/Bag.hpp"

#include <algorithm>
#include <map>
#include <random>

std::optional<std::vector<int>> generateRandomConstrainedBag(int n, const std::vector<Constraint>& constraints) {
    std::map<int, int> fixed_pos_to_val; // Key: Index -> Value: Piece
    std::map<int, int> val_to_fixed_pos; // Key: Piece -> Value: Index (for lookahead)
    std::multimap<int, int> piece_must_be_after; // Key: Piece -> Value: Must be after this piece

    // 1. Process Constraints
    for (const auto& con : constraints) {
        if (con.pos != -1) {
            if (fixed_pos_to_val.count(con.pos) && fixed_pos_to_val[con.pos] != con.piece) return std::nullopt;

            fixed_pos_to_val[con.pos] = con.piece;
            val_to_fixed_pos[con.piece] = con.pos;
        }
        if (con.after != -1) {
            piece_must_be_after.insert({con.piece, con.after});
        }
    }

    std::vector<int> result;
    std::random_device rd;
    std::mt19937 g(rd());

    for (int i = 0; i < n; i++) {
        std::vector<int> possible;

        if (fixed_pos_to_val.count(i)) {
            int target = fixed_pos_to_val[i];

            auto range = piece_must_be_after.equal_range(target);
            for (auto it = range.first; it != range.second; ++it) {
                int dependency = it->second;
                if (std::find(result.begin(), result.end(), dependency) == result.end()) {
                    return std::nullopt;
                }
            }
            result.push_back(target);
            continue;
        }

        for (int num = 0; num < n; num++) {
            if (std::find(result.begin(), result.end(), num) != result.end()) continue;
            if (val_to_fixed_pos.count(num) && val_to_fixed_pos[num] != i) continue;

            bool dependencies_met = true;
            auto range = piece_must_be_after.equal_range(num);
            for (auto it = range.first; it != range.second; ++it) {
                int dependency = it->second;
                if (std::find(result.begin(), result.end(), dependency) == result.end()) {
                    dependencies_met = false;
                    break;
                }
            }

            if (dependencies_met) {
                possible.push_back(num);
            }
        }

        if (possible.empty()) return std::nullopt;

        std::uniform_int_distribution<> dis(0, possible.size() - 1);
        int chosen = possible[dis(g)];
        result.push_back(chosen);
    }

    return result;
}
