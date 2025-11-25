#include "game/battle_simulator.hpp"
#include "game/tech_catalog.hpp"

#include <cassert>
#include <vector>

using namespace eclipse;

namespace {
const ShipDesign* findDesign(std::string_view id) {
    for (const ShipDesign& design : TechCatalog::shipDesigns()) {
        if (design.id == id) {
            return &design;
        }
    }
    return nullptr;
}
}

int main() {
    const ShipDesign* interceptor = findDesign("HUM_INT");
    assert(interceptor && "human interceptor design must exist");
    const ModuleSpec* drive = TechCatalog::findModule("NUCLEAR_DRIVE");
    const ModuleSpec* missile = TechCatalog::findModule("ANCIENT_MISSILE");
    assert(drive && missile);

    ShipLoadout missileShip(interceptor);
    missileShip.setModule(3, missile);  // free slot gains missile

    ShipLoadout vanillaShip(interceptor);

    BattleSimulator simulator;
    BattleSummary summary = simulator.simulate({missileShip}, {vanillaShip});

    assert(summary.humanWin > summary.alienWin);
    assert(summary.humanWin > 0.5);
    assert(summary.draw >= 0.0);

    return 0;
}
