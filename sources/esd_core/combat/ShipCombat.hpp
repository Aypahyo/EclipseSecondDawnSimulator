//
// Created by aypahyo on 10.08.21.
//

#ifndef SOURCES_ESD_CORE_COMBAT_SHIPCOMBAT_HPP_
#define SOURCES_ESD_CORE_COMBAT_SHIPCOMBAT_HPP_

#include <esd_core/data/Fleet.hpp>
#include <esd_core/data/CombatState.hpp>
#include <esd_core/data/InitiativeOrder.hpp>
#include <esd_core/data/DiceSet.hpp>

namespace esd {
struct ShipCombat {
  static CombatState MissileRound(CombatState startState, const esd::Fleet &attacker, const esd::Fleet &defender);
  static CombatState EngagementRound(
      CombatState startState,
      const esd::Fleet &attacker,
      const esd::Fleet &defender,
      char order_id,
      const esd::DiceSet& dice,
      long outcome);
  static InitiativeOrder GetTurnOrder(const esd::Fleet &attacker, const esd::Fleet &defender);
  static esd::DiceSet GetTotalEngagementDice(const esd::CombatState &state, const esd::Fleet &attacker, const esd::Fleet &defender, char order_id);
  static esd::DiceSet GetTotalMissileDice(const esd::CombatState &state, const esd::Fleet &attacker, const esd::Fleet &defender, char order_id);
};
}// namespace esd

#endif//SOURCES_ESD_CORE_COMBAT_SHIPCOMBAT_HPP_
