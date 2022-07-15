//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_COMBATSTATE_HPP_
#define SOURCES_ESD_CORE_DATA_COMBATSTATE_HPP_
#include <esd_core/data/Fleet.hpp>

namespace esd {
struct CombatState {
  char attacker_damage[4];
  char attacker_ships[4];
  char defender_damage[4];
  char defender_ships[4];
  long outcomes;
  long total_outcomes;
  bool Equals(const esd::CombatState &other) const {
    static_assert(sizeof(char) * 4 == sizeof(int));
    return
    *(int*)attacker_damage ==*(int*)other.attacker_damage &&
    *(int*)attacker_ships ==*(int*)other.attacker_ships &&
    *(int*)defender_damage ==*(int*)other.defender_damage &&
    *(int*)defender_ships ==*(int*)other.defender_ships;
  }
};

static_assert(std::is_trivial<CombatState>());
}
#endif//SOURCES_ESD_CORE_DATA_COMBATSTATE_HPP_
