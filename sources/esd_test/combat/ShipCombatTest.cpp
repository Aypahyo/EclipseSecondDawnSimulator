//
// Created by aypahyo on 10.08.21.
//
#include <esd_core/combat/ShipCombat.hpp>
#include <esd_core/data/ShipParts.hpp>
#include <esd_core/combat/CreateResults.hpp>
#include <esd_core/combat/CombatStateAggregator.hpp>
#include <gtest/gtest.h>

TEST(shipcombat, getTurnOrder_simple) {
  using namespace esd;
  auto attacker = esd::Fleet{}.Add(esd::ShipFightStats{}.Add(default_nuclear_source).Add(default_nuclear_drive).Add(default_ion_cannon));
  auto defender = esd::Fleet{}.Add(esd::ShipFightStats{}.Add(default_nuclear_source).Add(default_nuclear_drive).Add(default_ion_cannon));
  auto cmbt = esd::ShipCombat::GetTurnOrder(attacker, defender);
  ASSERT_EQ(cmbt.turnorder[0], InitiativeOrder_Defender + 0);
  ASSERT_EQ(cmbt.turnorder[1], InitiativeOrder_Attacker + 0);
}

TEST(shipcombat, getTurnOrder_complex) {
  using namespace esd;
  auto attacker = esd::Fleet{}
                      .Add(esd::ShipFightStats{}.Add(military_tachyon_source).Add(nano_fusion_drive).Add(default_ion_cannon)) //2
                      .Add(esd::ShipFightStats{}.Add(default_nuclear_source).Add(default_nuclear_drive).Add(default_ion_cannon)); //1
  auto defender = esd::Fleet{}
                      .Add(esd::ShipFightStats{}.Add(default_nuclear_source).Add(default_nuclear_drive).Add(default_ion_cannon)) //1
                      .Add(esd::ShipFightStats{}.Add(military_tachyon_source).Add(nano_fusion_drive).Add(default_nuclear_drive).Add(default_ion_cannon)); //3
  auto cmbt = esd::ShipCombat::GetTurnOrder(attacker, defender);
  ASSERT_EQ(cmbt.turnorder[0], InitiativeOrder_Defender + 1);
  ASSERT_EQ(cmbt.turnorder[1], InitiativeOrder_Attacker + 0);
  ASSERT_EQ(cmbt.turnorder[2], InitiativeOrder_Defender + 0);
  ASSERT_EQ(cmbt.turnorder[3], InitiativeOrder_Attacker + 1);
}

TEST(shipcombat, GetTotalEngagementDice_Simple) {
  using namespace esd;
  auto attacker = esd::Fleet{}
  .Add(esd::ShipFightStats{}.Add(military_plasma_cannon).Add(default_ion_cannon));
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
  .Add(esd::ShipFightStats{}.Add(military_plasma_cannon).Add(default_ion_cannon).Add(default_nuclear_drive));
  auto defender = esd::Fleet{}
  .Add(esd::ShipFightStats{}.Add(grid_improved_hull).Add(grid_improved_hull).Add(grid_improved_hull).Add(grid_improved_hull));
  auto state = esd::CombatState{};
  state.attacker_ships[0] = 2;
  state.defender_ships[0] = 1;
  auto turn_order = esd::ShipCombat::GetTurnOrder(attacker, defender);
  auto dice = esd::ShipCombat::GetTotalEngagementDice(state, attacker, defender, turn_order.turnorder[0]);
  auto total_outcomes = esd::CreateResults::GetTotalOutcomes(dice.total());
  for (long outcome = 0; outcome < total_outcomes; ++outcome) {
    auto newState = esd::ShipCombat::EngagementRound(state, attacker, defender, turn_order.turnorder[0], dice, outcome);
    std::cout << "damage: " << (int)newState.defender_damage[0] << " outcome: " << outcome << "\n";
  }
}

TEST(shipcombat, processEngagement_aggregateResults) {
  using namespace esd;
  auto attacker = esd::Fleet{}
  .Add(esd::ShipFightStats{}.Add(military_plasma_cannon).Add(default_ion_cannon).Add(default_nuclear_drive));
  auto defender = esd::Fleet{}
  .Add(esd::ShipFightStats{}.Add(grid_improved_hull).Add(grid_improved_hull).Add(grid_improved_hull).Add(grid_improved_hull));
  auto state = esd::CombatState{};
  state.attacker_ships[0] = 5;
  state.defender_ships[0] = 1;
  auto turn_order = esd::ShipCombat::GetTurnOrder(attacker, defender);
  auto dice = esd::ShipCombat::GetTotalEngagementDice(state, attacker, defender, turn_order.turnorder[0]);
  auto total_outcomes = esd::CreateResults::GetTotalOutcomes(dice.total());
  auto aggregator = esd::CombatStateAggregator{};
  state.outcomes = 1;
  state.total_outcomes = total_outcomes;
  for (long outcome = 0; outcome < total_outcomes; ++outcome) {
    auto newState = esd::ShipCombat::EngagementRound(state, attacker, defender, turn_order.turnorder[0], dice, outcome);
    aggregator.Add(newState);
  }
  for (auto aggState : aggregator.states) {
    std::cout << "damage: " << (int)aggState.defender_damage[0] << " " << (int)((double)aggState.outcomes * 100.0 / (double)aggState.total_outcomes) << "% \n";
  }
}