#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace eclipse {

enum class Faction {
    Human,
    Eridani,
    Planta,
    Orion
};

enum class SlotType {
    Weapon,
    Drive,
    Computer,
    Shield,
    Power,
    Support
};

enum class ShipClass {
    Interceptor,
    Cruiser,
    Dreadnought,
    Starbase,
    Other
};

struct ModuleSpec {
    std::string id;
    std::string shortLabel;
    std::string name;
    SlotType slot;
    int energyCost = 0;
    int energyProvided = 0;
    int dice = 0;
    int accuracyBonus = 0;
    int shieldBonus = 0;
    int hullBonus = 0;
    int drivePower = 0;
    int weaponInitiative = 1;
    int weaponDieSides = 6;
    int baseToHit = 6;
    bool missile = false;
    bool oneShot = false;
    bool grantsFluxShield = false;
    bool blueprintOnly = false;
};

struct SlotBlueprint {
    SlotType preferredType = SlotType::Support;
    const ModuleSpec* preprint = nullptr;
};

struct ShipDesign {
    std::string id;
    std::string name;
    Faction faction = Faction::Human;
    ShipClass shipClass = ShipClass::Interceptor;
    int baseHull = 1;
    int baseComputer = 0;
    int baseShield = 0;
    int baseDice = 0;
    int baseEnergy = 0;
    int baseWeaponInitiative = 1;
    int baseWeaponDieSides = 6;
    int baseWeaponHit = 6;
    int baseInitiativeBonus = 0;
    bool drivesAllowed = true;
    bool requiresDrive = true;
    std::vector<SlotBlueprint> slots;
    std::vector<const ModuleSpec*> extras;
};

struct ShipDerivedStats {
    int hull = 0;
    int dice = 0;
    int computer = 0;
    int shield = 0;
    int energyAvailable = 0;
    int energyUsed = 0;
    int driveCount = 0;
    int initiativeBonus = 0;
};

class ShipLoadout {
public:
    ShipLoadout() = default;

    explicit ShipLoadout(const ShipDesign* design)
        : design_(design), modules_(design ? design->slots.size() : 0, nullptr) {}

    void setDesign(const ShipDesign* design) {
        design_ = design;
        modules_.assign(design ? design->slots.size() : 0, nullptr);
    }

    const ShipDesign* design() const { return design_; }

    size_t slotCount() const { return modules_.size(); }

    const ModuleSpec* moduleAt(size_t index) const {
        if (index >= modules_.size()) {
            return nullptr;
        }
        return modules_[index];
    }

    void setModule(size_t index, const ModuleSpec* module) {
        if (!module || !isSlotCompatible(index, *module)) {
            return;
        }
        if (index < modules_.size()) {
            modules_[index] = module;
        }
    }

    const ModuleSpec* preprintAt(size_t index) const {
        const SlotBlueprint* slot = slotBlueprint(index);
        return slot ? slot->preprint : nullptr;
    }

    const ModuleSpec* activeModuleAt(size_t index) const {
        if (index >= modules_.size()) {
            return preprintAt(index);
        }
        if (modules_[index]) {
            return modules_[index];
        }
        return preprintAt(index);
    }

    std::vector<const ModuleSpec*> activeModules(bool includeExtras = true) const {
        std::vector<const ModuleSpec*> active;
        if (!design_) {
            return active;
        }
        if (includeExtras) {
            for (const ModuleSpec* extra : design_->extras) {
                if (extra) {
                    active.push_back(extra);
                }
            }
        }
        for (size_t i = 0; i < modules_.size(); ++i) {
            if (const ModuleSpec* module = activeModuleAt(i)) {
                active.push_back(module);
            }
        }
        return active;
    }

    void clearModule(size_t index) {
        if (index < modules_.size()) {
            modules_[index] = nullptr;
        }
    }

    bool isSlotCompatible(size_t index, const ModuleSpec& module) const {
        if (!design_ || index >= modules_.size()) {
            return false;
        }
        if (!design_->drivesAllowed && module.slot == SlotType::Drive) {
            return false;
        }
        return true;
    }

    ShipDerivedStats derivedStats() const {
        ShipDerivedStats stats;
        if (!design_) {
            return stats;
        }
        stats.hull = design_->baseHull;
        stats.dice = design_->baseDice;
        stats.computer = design_->baseComputer;
        stats.shield = design_->baseShield;
        stats.energyAvailable = design_->baseEnergy;
        stats.energyUsed = 0;
        stats.driveCount = 0;
        stats.initiativeBonus = design_->baseInitiativeBonus;

        auto accumulate = [&](const ModuleSpec* module) {
            if (!module) {
                return;
            }
            stats.hull += module->hullBonus;
            stats.dice += module->dice;
            stats.computer += module->accuracyBonus;
            stats.shield += module->shieldBonus;
            stats.energyAvailable += module->energyProvided;
            stats.energyUsed += module->energyCost;
            if (module->drivePower > 0) {
                stats.driveCount += module->drivePower;
            } else if (module->slot == SlotType::Drive) {
                stats.driveCount += 1;
            }
        };

        for (const ModuleSpec* extra : design_->extras) {
            accumulate(extra);
        }
        for (size_t i = 0; i < modules_.size(); ++i) {
            accumulate(activeModuleAt(i));
        }
        return stats;
    }

    bool hasEngine() const {
        if (!design_) {
            return false;
        }
        if (!design_->requiresDrive) {
            return true;
        }
        auto drivePresent = [](const ModuleSpec* module) {
            return module && (module->slot == SlotType::Drive || module->drivePower > 0);
        };
        for (const ModuleSpec* extra : design_->extras) {
            if (drivePresent(extra)) {
                return true;
            }
        }
        for (size_t i = 0; i < modules_.size(); ++i) {
            if (drivePresent(activeModuleAt(i))) {
                return true;
            }
        }
        return false;
    }

    bool satisfiesEnergy() const {
        ShipDerivedStats stats = derivedStats();
        return stats.energyAvailable >= stats.energyUsed;
    }

    bool isValid() const {
        if (!design_) {
            return false;
        }
        if (hasIllegalDrive()) {
            return false;
        }
        if (design_->requiresDrive && !hasEngine()) {
            return false;
        }
        return satisfiesEnergy();
    }

    std::string validationError() const {
        if (!design_) {
            return "SELECT A DESIGN";
        }
        if (hasIllegalDrive()) {
            return "NO DRIVES ON STARBASE";
        }
        if (design_->requiresDrive && !hasEngine()) {
            return "NEEDS ENGINE";
        }
        if (!satisfiesEnergy()) {
            return "LOW ENERGY";
        }
        return "";
    }

    const std::vector<const ModuleSpec*>& modules() const { return modules_; }
    std::vector<const ModuleSpec*>& modules() { return modules_; }

private:
    const ShipDesign* design_ = nullptr;
    std::vector<const ModuleSpec*> modules_;

    const SlotBlueprint* slotBlueprint(size_t index) const {
        if (!design_ || index >= design_->slots.size()) {
            return nullptr;
        }
        return &design_->slots[index];
    }

    bool hasIllegalDrive() const {
        if (!design_ || design_->drivesAllowed) {
            return false;
        }
        for (size_t i = 0; i < modules_.size(); ++i) {
            const ModuleSpec* module = activeModuleAt(i);
            if (module && (module->slot == SlotType::Drive || module->drivePower > 0)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace eclipse
