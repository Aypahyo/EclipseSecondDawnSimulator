#include "game/tech_catalog.hpp"

#include <algorithm>
#include <utility>
#include <stdexcept>

namespace eclipse {

namespace {
std::vector<ModuleSpec> buildModules() {
    std::vector<ModuleSpec> modules;

    auto weapon = [&](std::string id, std::string shortLabel, std::string name, int energyCost,
                      int dice, int baseToHit, int initiative, int dieSides, bool missile = false,
                      bool oneShot = false) {
        ModuleSpec spec;
        spec.id = std::move(id);
        spec.shortLabel = std::move(shortLabel);
        spec.name = std::move(name);
        spec.slot = SlotType::Weapon;
        spec.energyCost = energyCost;
        spec.dice = dice;
        spec.baseToHit = baseToHit;
        spec.weaponInitiative = initiative;
        spec.weaponDieSides = dieSides;
        spec.missile = missile;
        spec.oneShot = oneShot;
        modules.push_back(spec);
    };

    auto computer = [&](std::string id, std::string shortLabel, std::string name, int energyCost, int bonus) {
        ModuleSpec spec;
        spec.id = std::move(id);
        spec.shortLabel = std::move(shortLabel);
        spec.name = std::move(name);
        spec.slot = SlotType::Computer;
        spec.energyCost = energyCost;
        spec.accuracyBonus = bonus;
        modules.push_back(spec);
    };

    auto shield = [&](std::string id, std::string shortLabel, std::string name, int energyCost, int bonus) {
        ModuleSpec spec;
        spec.id = std::move(id);
        spec.shortLabel = std::move(shortLabel);
        spec.name = std::move(name);
        spec.slot = SlotType::Shield;
        spec.energyCost = energyCost;
        spec.shieldBonus = bonus;
        modules.push_back(spec);
    };

    auto hull = [&](std::string id, std::string shortLabel, std::string name, int energyCost, int bonus) {
        ModuleSpec spec;
        spec.id = std::move(id);
        spec.shortLabel = std::move(shortLabel);
        spec.name = std::move(name);
        spec.slot = SlotType::Support;
        spec.energyCost = energyCost;
        spec.hullBonus = bonus;
        modules.push_back(spec);
    };

    auto support = [&](std::string id, std::string shortLabel, std::string name, SlotType slot,
                       int energyCost, int energyProvided, int drivePower = 0, bool flux = false) {
        ModuleSpec spec;
        spec.id = std::move(id);
        spec.shortLabel = std::move(shortLabel);
        spec.name = std::move(name);
        spec.slot = slot;
        spec.energyCost = energyCost;
        spec.energyProvided = energyProvided;
        spec.drivePower = drivePower;
        spec.grantsFluxShield = flux;
        modules.push_back(spec);
    };

        auto blueprintSupport = [&](std::string id, std::string shortLabel, std::string name, SlotType slot,
                                    int energyCost, int energyProvided) {
            ModuleSpec spec;
            spec.id = std::move(id);
            spec.shortLabel = std::move(shortLabel);
            spec.name = std::move(name);
            spec.slot = slot;
            spec.energyCost = energyCost;
            spec.energyProvided = energyProvided;
            spec.blueprintOnly = true;
            modules.push_back(spec);
        };

        auto blueprintShield = [&](std::string id, std::string shortLabel, std::string name, int bonus) {
            ModuleSpec spec;
            spec.id = std::move(id);
            spec.shortLabel = std::move(shortLabel);
            spec.name = std::move(name);
            spec.slot = SlotType::Shield;
            spec.shieldBonus = bonus;
            spec.blueprintOnly = true;
            modules.push_back(spec);
        };

    // Weapons
    weapon("ION_CANNON", "ION", "Ion Cannon", 1, 1, 6, 1, 6);
    weapon("PLASMA_CANNON", "PLAS", "Plasma Cannon", 2, 1, 6, 2, 6);
    weapon("POSITRON_CANNON", "POSC", "Positron Cannon", 3, 1, 6, 2, 8);
    weapon("ANTIMATTER_CANNON", "ANTI", "Antimatter Cannon", 3, 1, 5, 3, 6);
    weapon("NUCLEAR_MISSILE", "NMIS", "Nuclear Missile", 2, 1, 6, 0, 6, true, true);
    weapon("PLASMA_MISSILE", "PMIS", "Plasma Missile", 3, 1, 6, 0, 8, true, true);
    weapon("ANCIENT_CANNON", "ACAN", "Ancient Technology Cannon", 0, 1, 5, 2, 6);
    weapon("ANCIENT_TURRET", "ATUR", "Ancient Ion Turret", 0, 1, 6, 1, 6);
    weapon("ANCIENT_MISSILE", "AMIS", "Ancient Missile", 0, 1, 5, 0, 6, true, true);

    // Drives
    support("NUCLEAR_DRIVE", "NUKE", "Nuclear Drive", SlotType::Drive, 0, 0, 1);
    support("FUSION_DRIVE", "FUSN", "Fusion Drive", SlotType::Drive, 1, 0, 2);
    support("ION_DRIVE", "IOND", "Ion Drive", SlotType::Drive, 2, 0, 2);

    // Computers
    computer("ELECTRON_COMPUTER", "EC+1", "Electron Computer", 1, 1);
    computer("POSITRON_COMPUTER", "PC+2", "Positron Computer", 2, 2);
    computer("GLUON_COMPUTER", "GC+3", "Gluon Computer", 3, 3);
    computer("ANCIENT_COMPUTER", "AC+2", "Ancient Computer", 0, 2);

    // Shields
    shield("GAUSS_SHIELD", "SH+1", "Gauss Shield", 1, 1);
    shield("PHASE_SHIELD", "SH+2", "Phase Shield", 2, 2);
    shield("ADVANCED_SHIELD", "SH+3", "Advanced Shield", 3, 3);
    shield("ANCIENT_SHIELD", "ASH+2", "Ancient Shield", 0, 2);

    // Hull & support
    hull("HULL", "H+1", "Hull", 1, 1);
    hull("IMPROVED_HULL", "H+2", "Improved Hull", 2, 2);
    hull("ANCIENT_HULL", "AH+1", "Ancient Hull", 0, 1);
    support("FLUX_SHIELD", "FLUX", "Flux Shield", SlotType::Support, 2, 0, 0, true);

    // Power sources / reactors / grids
    support("FUSION_SOURCE", "PWR2", "Fusion Source", SlotType::Power, 0, 2);
    support("ANTIMATTER_REACTOR", "PWR3", "Antimatter Reactor", SlotType::Power, 0, 3);
    support("DARK_MATRIX", "DMAT", "Dark Energy Matrix", SlotType::Power, 0, 4);
    support("IMPROVED_REACTOR", "PWR1", "Improved Reactor", SlotType::Power, 0, 1);
    support("QUANTUM_GRID", "QGRD", "Quantum Grid", SlotType::Support, 0, 1);

    // Blueprint-only printed parts
    blueprintSupport("BASIC_REACTOR", "PWR3", "Basic Reactor", SlotType::Power, 0, 3);
    blueprintSupport("BLUEPRINT_REACTOR_5", "E+5", "Blueprint Energy +5", SlotType::Power, 0, 5);
    blueprintShield("ORION_SHIELD_MALUS", "SH-1", "Orion Shield Debt", -1);

    return modules;
}

const ModuleSpec* requireModule(std::string_view id) {
    const ModuleSpec* spec = TechCatalog::findModule(id);
    if (!spec) {
        throw std::runtime_error("Missing module spec: " + std::string(id));
    }
    return spec;
}

SlotBlueprint makeSlot(SlotType type, const ModuleSpec* preprint) {
    SlotBlueprint slot;
    slot.preferredType = type;
    slot.preprint = preprint;
    return slot;
}

std::vector<ShipDesign> buildDesigns() {
    std::vector<ShipDesign> designs;

    const ModuleSpec* ion = requireModule("ION_CANNON");
    const ModuleSpec* drive = requireModule("NUCLEAR_DRIVE");
    const ModuleSpec* basicReactor = requireModule("BASIC_REACTOR");
    const ModuleSpec* electron = requireModule("ELECTRON_COMPUTER");
    const ModuleSpec* hull = requireModule("HULL");
    const ModuleSpec* improvedReactor = requireModule("IMPROVED_REACTOR");
    const ModuleSpec* fusionSource = requireModule("FUSION_SOURCE");
    const ModuleSpec* reactorFive = requireModule("BLUEPRINT_REACTOR_5");
    const ModuleSpec* shieldPenalty = requireModule("ORION_SHIELD_MALUS");

    auto makeDesign = [&](std::string id, std::string name, Faction faction, ShipClass cls,
                          int baseHull, int baseComputer, int baseShield, int initiativeBonus,
                          bool drivesAllowed, bool requiresDrive,
                          std::vector<SlotBlueprint> slots,
                          std::vector<const ModuleSpec*> extras) {
        ShipDesign design;
        design.id = std::move(id);
        design.name = std::move(name);
        design.faction = faction;
        design.shipClass = cls;
        design.baseHull = baseHull;
        design.baseComputer = baseComputer;
        design.baseShield = baseShield;
        design.baseDice = 0;
        design.baseEnergy = 0;
        design.baseWeaponInitiative = 1;
        design.baseWeaponDieSides = 6;
        design.baseWeaponHit = 6;
        design.baseInitiativeBonus = initiativeBonus;
        design.drivesAllowed = drivesAllowed;
        design.requiresDrive = requiresDrive;
        design.slots = std::move(slots);
        design.extras = std::move(extras);
        designs.push_back(std::move(design));
    };

    auto terranInterceptorSlots = [&]() {
        return std::vector<SlotBlueprint>{
            makeSlot(SlotType::Weapon, ion),
            makeSlot(SlotType::Drive, drive),
            makeSlot(SlotType::Power, basicReactor),
            makeSlot(SlotType::Support, nullptr),
        };
    };

    auto terranCruiserSlots = [&]() {
        return std::vector<SlotBlueprint>{
            makeSlot(SlotType::Weapon, ion),
            makeSlot(SlotType::Drive, drive),
            makeSlot(SlotType::Power, basicReactor),
            makeSlot(SlotType::Computer, electron),
            makeSlot(SlotType::Support, hull),
            makeSlot(SlotType::Support, nullptr),
        };
    };

    auto terranDreadSlots = [&]() {
        return std::vector<SlotBlueprint>{
            makeSlot(SlotType::Weapon, ion),
            makeSlot(SlotType::Weapon, ion),
            makeSlot(SlotType::Drive, drive),
            makeSlot(SlotType::Power, basicReactor),
            makeSlot(SlotType::Computer, electron),
            makeSlot(SlotType::Support, hull),
            makeSlot(SlotType::Support, hull),
            makeSlot(SlotType::Support, nullptr),
        };
    };

    auto terranStarbaseSlots = [&]() {
        return std::vector<SlotBlueprint>{
            makeSlot(SlotType::Computer, electron),
            makeSlot(SlotType::Weapon, ion),
            makeSlot(SlotType::Support, hull),
            makeSlot(SlotType::Support, hull),
            makeSlot(SlotType::Support, nullptr),
        };
    };

    makeDesign("HUM_INT", "Human Interceptor", Faction::Human, ShipClass::Interceptor,
               1, 0, 0, 2, true, true, terranInterceptorSlots(), {});
    makeDesign("HUM_CRU", "Human Cruiser", Faction::Human, ShipClass::Cruiser,
               2, 0, 0, 1, true, true, terranCruiserSlots(), {});
    makeDesign("HUM_DRE", "Human Dreadnought", Faction::Human, ShipClass::Dreadnought,
               3, 0, 0, 0, true, true, terranDreadSlots(), {});
    makeDesign("HUM_STA", "Human Starbase", Faction::Human, ShipClass::Starbase,
               4, 0, 0, 5, false, false, terranStarbaseSlots(), {basicReactor});

    size_t humanCount = designs.size();

    auto labelSuffix = [](const ShipDesign& design) {
        size_t pos = design.name.find(' ');
        if (pos == std::string::npos) {
            return design.name;
        }
        return design.name.substr(pos + 1);
    };

    auto cloneWith = [&](const ShipDesign& base, std::string id, std::string name, Faction faction) {
        ShipDesign copy = base;
        copy.id = std::move(id);
        copy.name = std::move(name);
        copy.faction = faction;
        return copy;
    };

    auto pushDesign = [&](ShipDesign design) {
        designs.push_back(std::move(design));
    };

    // Eridani Empire modifications
    for (size_t i = 0; i < humanCount; ++i) {
        const ShipDesign& base = designs[i];
    ShipDesign eri = cloneWith(base, std::string("ERI_") + base.id.substr(4),
                   "Eridani " + labelSuffix(base), Faction::Eridani);
        if (eri.shipClass != ShipClass::Starbase) {
            eri.extras.push_back(improvedReactor);
        }
        pushDesign(std::move(eri));
    }

    // Planta modifications
    for (size_t i = 0; i < humanCount; ++i) {
        const ShipDesign& base = designs[i];
    ShipDesign pla = cloneWith(base, std::string("PLA_") + base.id.substr(4),
                   "Planta " + labelSuffix(base), Faction::Planta);
        pla.baseInitiativeBonus = (pla.shipClass == ShipClass::Starbase) ? 2 : 0;
        if (pla.shipClass == ShipClass::Starbase) {
            pla.extras = {electron, reactorFive};
            bool cleared = false;
            for (auto& slot : pla.slots) {
                if (!cleared && slot.preprint == hull) {
                    slot.preprint = nullptr;
                    cleared = true;
                }
            }
        } else {
            pla.extras.push_back(electron);
            pla.extras.push_back(improvedReactor);
            bool cleared = false;
            for (auto& slot : pla.slots) {
                if (!cleared && slot.preprint == basicReactor) {
                    slot.preprint = nullptr;
                    cleared = true;
                }
            }
            if (!cleared) {
                for (auto& slot : pla.slots) {
                    if (slot.preprint && slot.preprint != ion && slot.preprint != drive) {
                        slot.preprint = nullptr;
                        break;
                    }
                }
            }
        }
        pushDesign(std::move(pla));
    }

    // Orion Hegemony modifications
    for (size_t i = 0; i < humanCount; ++i) {
        const ShipDesign& base = designs[i];
    ShipDesign ori = cloneWith(base, std::string("ORI_") + base.id.substr(4),
                   "Orion " + labelSuffix(base), Faction::Orion);
        switch (ori.shipClass) {
            case ShipClass::Interceptor:
                ori.baseInitiativeBonus = 3;
                ori.extras.push_back(improvedReactor);
                break;
            case ShipClass::Cruiser:
                ori.baseInitiativeBonus = 2;
                ori.extras.push_back(fusionSource);
                break;
            case ShipClass::Dreadnought:
                ori.baseInitiativeBonus = 1;
                ori.extras.push_back(basicReactor);
                break;
            case ShipClass::Starbase:
                ori.baseInitiativeBonus = 5;
                break;
            default:
                break;
        }
        for (auto& slot : ori.slots) {
            if (!slot.preprint) {
                slot.preprint = shieldPenalty;
            }
        }
        pushDesign(std::move(ori));
    }

    return designs;
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
