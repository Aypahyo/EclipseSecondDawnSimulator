#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace eclipse {

enum class Faction {
    Human,
    Alien
};

enum class SlotType {
    Weapon,
    Drive,
    Computer,
    Shield,
    Power,
    Support
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
};

struct ShipDesign {
    std::string id;
    std::string name;
    Faction faction = Faction::Human;
    int baseHull = 1;
    int baseComputer = 0;
    int baseShield = 0;
    int baseDice = 0;
    int baseEnergy = 0;
    std::vector<SlotType> slots;
};

struct ShipDerivedStats {
    int hull = 0;
    int dice = 0;
    int computer = 0;
    int shield = 0;
    int energyAvailable = 0;
    int energyUsed = 0;
    int driveCount = 0;
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
        if (index < modules_.size()) {
            modules_[index] = module;
        }
    }

    void clearModule(size_t index) {
        if (index < modules_.size()) {
            modules_[index] = nullptr;
        }
    }

    bool isSlotCompatible(size_t index, const ModuleSpec& module) const {
        if (!design_ || index >= design_->slots.size()) {
            return false;
        }
        return design_->slots[index] == module.slot;
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

        for (const ModuleSpec* module : modules_) {
            if (!module) {
                continue;
            }
            stats.hull += module->hullBonus;
            stats.dice += module->dice;
            stats.computer += module->accuracyBonus;
            stats.shield += module->shieldBonus;
            stats.energyAvailable += module->energyProvided;
            stats.energyUsed += module->energyCost;
            if (module->slot == SlotType::Drive) {
                stats.driveCount += 1;
            }
        }
        return stats;
    }

    bool hasEngine() const {
        for (const ModuleSpec* module : modules_) {
            if (module && module->slot == SlotType::Drive) {
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
        if (!hasEngine()) {
            return false;
        }
        return satisfiesEnergy();
    }

    std::string validationError() const {
        if (!design_) {
            return "SELECT A DESIGN";
        }
        if (!hasEngine()) {
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
};

}  // namespace eclipse
