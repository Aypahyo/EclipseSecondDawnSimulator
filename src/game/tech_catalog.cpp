#include "game/tech_catalog.hpp"

#include <algorithm>

namespace eclipse {

namespace {
std::vector<ModuleSpec> buildModules() {
    return {
        {"ION_CANNON", "ION", "Ion Cannon", SlotType::Weapon, 1, 0, 1, 0, 0, 0, 0},
        {"PLASMA_CANNON", "PLAS", "Plasma Cannon", SlotType::Weapon, 2, 0, 1, 1, 0, 0, 0},
        {"DISRUPTOR", "DISR", "Disruptor", SlotType::Weapon, 3, 0, 2, 0, 0, 0, 0},
        {"GAUSS_CANNON", "GAUS", "Gauss Cannon", SlotType::Weapon, 2, 0, 2, 0, 0, 0, 0},
        {"NUCLEAR_DRIVE", "NUKE", "Nuclear Drive", SlotType::Drive, 0, 0, 0, 0, 0, 0, 1},
        {"FUSION_DRIVE", "FUSN", "Fusion Drive", SlotType::Drive, 1, 0, 0, 0, 0, 0, 2},
        {"ION_DRIVE", "IOND", "Ion Drive", SlotType::Drive, 2, 0, 0, 0, 0, 0, 2},
        {"NANO_COMPUTER", "NANO", "Nano Computer", SlotType::Computer, 1, 0, 0, 1, 0, 0, 0},
        {"POSITRON_COMPUTER", "POSI", "Positron Computer", SlotType::Computer, 2, 0, 0, 2, 0, 0, 0},
        {"ANCIENT_COMPUTER", "ANCI", "Ancient Computer", SlotType::Computer, 3, 0, 0, 3, 0, 0, 0},
        {"GAUSS_SHIELD", "SH+1", "Gauss Shield", SlotType::Shield, 1, 0, 0, 0, 1, 0, 0},
        {"PHASE_SHIELD", "SH+2", "Phase Shield", SlotType::Shield, 2, 0, 0, 0, 2, 0, 0},
        {"FUSION_SOURCE", "PWR2", "Fusion Source", SlotType::Power, 0, 2, 0, 0, 0, 0, 0},
        {"ANTIMATTER_REACTOR", "PWR3", "Antimatter Reactor", SlotType::Power, 0, 3, 0, 0, 0, 0, 0},
        {"DARK_MATRIX", "DMAT", "Dark Energy Matrix", SlotType::Power, 0, 4, 0, 0, 0, 0, 0},
    };
}

std::vector<SlotType> slots(std::initializer_list<SlotType> order) {
    return std::vector<SlotType>(order);
}

std::vector<ShipDesign> buildDesigns() {
    return {
        {"HUM_INT", "Human Interceptor", Faction::Human, 1, 1, 0, 1, 1, slots({SlotType::Weapon, SlotType::Drive, SlotType::Computer})},
        {"HUM_CRU", "Human Cruiser", Faction::Human, 2, 1, 0, 1, 2,
         slots({SlotType::Weapon, SlotType::Weapon, SlotType::Drive, SlotType::Computer, SlotType::Shield, SlotType::Power})},
        {"HUM_DRE", "Human Dreadnought", Faction::Human, 3, 2, 0, 2, 3,
         slots({SlotType::Weapon, SlotType::Weapon, SlotType::Weapon, SlotType::Drive, SlotType::Computer, SlotType::Shield, SlotType::Power})},
        {"ALI_INT", "Alien Phantom", Faction::Alien, 1, 2, 0, 1, 1,
         slots({SlotType::Weapon, SlotType::Drive, SlotType::Computer})},
        {"ALI_CRU", "Alien Cruiser", Faction::Alien, 2, 2, 1, 2, 2,
         slots({SlotType::Weapon, SlotType::Weapon, SlotType::Drive, SlotType::Computer, SlotType::Shield, SlotType::Power})},
        {"ALI_DRE", "Alien Rift Dreadnought", Faction::Alien, 3, 2, 1, 2, 3,
         slots({SlotType::Weapon, SlotType::Weapon, SlotType::Weapon, SlotType::Drive, SlotType::Computer, SlotType::Shield, SlotType::Power, SlotType::Power})},
    };
}

}  // namespace

const std::vector<ModuleSpec>& TechCatalog::modules() {
    static std::vector<ModuleSpec> modules = buildModules();
    return modules;
}

const ModuleSpec* TechCatalog::findModule(std::string_view id) {
    const auto& items = modules();
    auto it = std::find_if(items.begin(), items.end(), [&](const ModuleSpec& spec) {
        return spec.id == id;
    });
    if (it == items.end()) {
        return nullptr;
    }
    return &(*it);
}

const std::vector<ShipDesign>& TechCatalog::shipDesigns() {
    static std::vector<ShipDesign> designs = buildDesigns();
    return designs;
}

std::vector<const ShipDesign*> TechCatalog::factionDesigns(Faction faction) {
    std::vector<const ShipDesign*> results;
    for (const ShipDesign& design : shipDesigns()) {
        if (design.faction == faction) {
            results.push_back(&design);
        }
    }
    return results;
}

}  // namespace eclipse
