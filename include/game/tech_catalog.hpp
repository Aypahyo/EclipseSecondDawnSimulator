#pragma once

#include <vector>
#include <string_view>

#include "game/types.hpp"

namespace eclipse {

class TechCatalog {
public:
    static const std::vector<ModuleSpec>& modules();
    static const ModuleSpec* findModule(std::string_view id);

    static const std::vector<ShipDesign>& shipDesigns();
    static std::vector<const ShipDesign*> factionDesigns(Faction faction);
};

}  // namespace eclipse
