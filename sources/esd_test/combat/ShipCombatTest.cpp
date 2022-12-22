//
// Created by aypahyo on 10.08.21.
//
#include <esd_core/combat/CombatStateAggregator.hpp>
#include <esd_core/combat/CreateResults.hpp>
#include <esd_core/combat/ShipCombat.hpp>
#include <esd_core/data/ShipBuilder.hpp>
#include <esd_core/data/ShipParts.hpp>
#include <gtest/gtest.h>

TEST(shipcombat, getTurnOrder_simple) {
  using namespace esd;
  auto attacker = esd::Fleet{}.Add(esd::ShipBuilder{}.w(default_nuclear_source).w(default_nuclear_drive).w(default_ion_cannon).stats());
  auto defender = esd::Fleet{}.Add(esd::ShipBuilder{}.w(default_nuclear_source).w(default_nuclear_drive).w(default_ion_cannon).stats());
  auto combat = esd::ShipCombat::GetTurnOrder(attacker, defender);
  ASSERT_EQ(combat.turn_order[0], InitiativeOrder_Defender + 0);
  ASSERT_EQ(combat.turn_order[1], InitiativeOrder_Attacker + 0);
}

TEST(shipcombat, getTurnOrder_complex) {
  using namespace esd;
  auto attacker = esd::Fleet{}
                      .Add(esd::ShipBuilder{}.w(blueprint_initiative_1).stats())
                      .Add(esd::ShipBuilder{}.w(blueprint_initiative_2).stats());
  auto defender = esd::Fleet{}
                      .Add(esd::ShipBuilder{}.w(blueprint_initiative_1).stats())
                      .Add(esd::ShipBuilder{}.w(blueprint_initiative_3).stats());
  auto combat = esd::ShipCombat::GetTurnOrder(attacker, defender);
  ASSERT_EQ(combat.turn_order[0], InitiativeOrder_Defender + 1);
  ASSERT_EQ(combat.turn_order[1], InitiativeOrder_Attacker + 0);
  ASSERT_EQ(combat.turn_order[2], InitiativeOrder_Defender + 0);
  ASSERT_EQ(combat.turn_order[3], InitiativeOrder_Attacker + 1);
}

TEST(shipcombat, GetTotalEngagementDice_Simple) {
  using namespace esd;
  auto attacker = esd::Fleet{}
                      .Add(esd::ShipBuilder{}.w(military_plasma_cannon).w(default_ion_cannon).stats());
  auto defender = esd::Fleet{};
  auto state = esd::CombatState{};
  state.attacker_ships[0] = 3;
  auto dice = esd::ShipCombat::GetTotalEngagementDice(state, attacker, defender, InitiativeOrder_Attacker + 0);
  ASSERT_EQ(dice.total(), 6);
  auto total_outcomes = esd::CreateResults::GetTotalOutcomes(dice.total());
  ASSERT_EQ(total_outcomes, 6*6*6*6*6*6);
}

TEST(shipcombat, processEngagement_outcomes) {
  using namespace esd;
  auto attacker = esd::Fleet{}
                      .Add(esd::ShipBuilder{}.w(default_ion_cannon).w(default_ion_cannon).stats());
  auto defender = esd::Fleet{}
                      .Add(esd::ShipBuilder{}.w(grid_improved_hull).stats());
  auto state = esd::CombatState{};
  state.attacker_ships[0] = 1;
  state.defender_ships[0] = 1;
  auto turn_order = esd::ShipCombat::GetTurnOrder(attacker, defender);
  auto dice = esd::ShipCombat::GetTotalEngagementDice(state, attacker, defender, turn_order.turn_order[0]);
  auto total_outcomes = esd::CreateResults::GetTotalOutcomes(dice.total());
  for (long outcome = 0; outcome < total_outcomes; ++outcome) {
    auto newState = esd::ShipCombat::EngagementRound(state, attacker, defender, turn_order.turn_order[0], dice, outcome);
    std::cout << "damage: " << (int)newState.defender_damage[0] << " outcome: " << outcome << "\n";
  }
}

TEST(shipcombat, processEngagement_aggregateResults) {
  using namespace esd;
  auto attacker = esd::Fleet{}
                      .Add(esd::ShipBuilder{}.w(military_plasma_cannon).stats());
  auto defender = esd::Fleet{}
                      .Add(esd::ShipBuilder{}.w(grid_improved_hull).stats());
  auto state = esd::CombatState{};
  state.attacker_ships[0] = 5;
  state.defender_ships[0] = 1;
  auto turn_order = esd::ShipCombat::GetTurnOrder(attacker, defender);
  auto dice = esd::ShipCombat::GetTotalEngagementDice(state, attacker, defender, turn_order.turn_order[0]);
  auto total_outcomes = esd::CreateResults::GetTotalOutcomes(dice.total());
  auto aggregator = esd::CombatStateAggregator{};
  state.outcomes = 1;
  state.total_outcomes = total_outcomes;
  for (long outcome = 0; outcome < total_outcomes; ++outcome) {
    auto newState = esd::ShipCombat::EngagementRound(state, attacker, defender, turn_order.turn_order[0], dice, outcome);
    aggregator.Add(newState);
  }
  std::cout << "--------------\n";
  for (auto aggState : aggregator.states) {
    std::cout << "damage: " << (int)aggState.defender_damage[0] << " " << (int)((double)aggState.outcomes * 100.0 / (double)aggState.total_outcomes) << "% \n";
  }
}