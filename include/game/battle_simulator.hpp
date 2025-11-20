#pragma once

#include <vector>

#include "game/types.hpp"

namespace eclipse {

struct BattleSummary {
    double humanWin = 0.0;
    double alienWin = 0.0;
    double draw = 0.0;
    double expectedRounds = 0.0;
};

class BattleSimulator {
public:
    BattleSummary simulate(const std::vector<ShipLoadout>& humans,
                           const std::vector<ShipLoadout>& aliens);
};

}  // namespace eclipse
