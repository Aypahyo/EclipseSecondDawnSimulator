//
// Created by aypahyo on 10.08.21.
//

#include "ShipCombat.hpp"
#include "CreateResults.hpp"

#include <vector>
#include <tuple>
#include <algorithm>

namespace esd {

CombatState ShipCombat::MissileRound(CombatState startState, const esd::Fleet &attacker, const esd::Fleet &defender)
{
        return startState;
}

CombatState ShipCombat::EngagementRound(
    CombatState startState,
    const esd::Fleet &attacker,
    const esd::Fleet &defender,
    char order_id,
    const esd::DiceSet& dice,
    long outcome)
{
  auto dice_outcome = CreateResults::getOutcome(dice.yellow, dice.orange, dice.blue, dice.red, outcome);

  for (int yellow = 0; yellow < dice_outcome.yellow_results_count; ++yellow) {
    auto f = dice_outcome.yellow_results(yellow);
    if(f == 6) startState.defender_damage[0] += 1;
  }

  for (int orange = 0; orange < dice_outcome.orange_results_count; ++orange) {
    auto f = dice_outcome.orange_results(orange);
    if(f == 6) startState.defender_damage[0] += 2;
  }

  for (int blue = 0; blue < dice_outcome.blue_results_count; ++blue) {
    auto f = dice_outcome.blue_results(blue);
    if(f == 6) startState.defender_damage[0] += 3;
  }

  for (int red = 0; red < dice_outcome.red_results_count; ++red) {
    auto f = dice_outcome.red_results(red);
    if(f == 6) startState.defender_damage[0] += 4;
  }

  return startState;
}

esd::DiceSet ShipCombat::GetTotalEngagementDice(const esd::CombatState &state, const esd::Fleet &attacker, const esd::Fleet &defender, char order_id)
{
  char ship_count;
  ShipFightStats ship_fight_stats;
  if (order_id < InitiativeOrder_Defender)
  {
    order_id -= InitiativeOrder_Attacker;
    ship_count = state.attacker_ships[order_id];
    ship_fight_stats = attacker.ships[order_id];
  }
  else
  {
    order_id -= InitiativeOrder_Defender;
    ship_count = state.defender_ships[order_id];
    ship_fight_stats = defender.ships[order_id];
  }
  DiceSet d{};
  d.yellow = ship_fight_stats.yellow_cannon * ship_count;
  d.orange = ship_fight_stats.orange_cannon * ship_count;
  d.blue = ship_fight_stats.blue_cannon * ship_count;
  d.red = ship_fight_stats.red_cannon * ship_count;
  return d;
}

esd::DiceSet ShipCombat::GetTotalMissileDice(const esd::CombatState &state, const esd::Fleet &attacker, const esd::Fleet &defender, char order_id)
{
  char ship_count;
  ShipFightStats ship_fight_stats;
  if (order_id < InitiativeOrder_Defender)
  {
    order_id -= InitiativeOrder_Attacker;
    ship_count = state.attacker_ships[order_id];
    ship_fight_stats = attacker.ships[order_id];
  }
  else
  {
    order_id -= InitiativeOrder_Defender;
    ship_count = state.defender_ships[order_id];
    ship_fight_stats = defender.ships[order_id];
  }
  DiceSet d{};
  d.yellow = ship_fight_stats.yellow_cannon * ship_count;
  d.orange = ship_fight_stats.orange_cannon * ship_count;
  d.blue = ship_fight_stats.blue_cannon * ship_count;
  d.red = ship_fight_stats.red_cannon * ship_count;
  return d;
}

InitiativeOrder ShipCombat::GetTurnOrder(const esd::Fleet &attacker, const esd::Fleet &defender)
{
  auto initiative_order = InitiativeOrder{};
  std::vector<std::tuple<char, char>> ship_id_initiative{};

  for (int i = 0; i < attacker.ships_count; ++i){
    ship_id_initiative.emplace_back(InitiativeOrder_Attacker + i, attacker.ships[i].initiative);
  }
  for (int i = 0; i < defender.ships_count; ++i) {
    ship_id_initiative.emplace_back(InitiativeOrder_Defender + i, defender.ships[i].initiative);
  }

  std::sort(ship_id_initiative.begin(), ship_id_initiative.end(), [](std::tuple<char, char> a, std::tuple<char, char> b)->bool{
    if(std::get<1>(a) == std::get<1>(b))
      return std::get<0>(a) > std::get<0>(b);
    return std::get<1>(a) > std::get<1>(b);
  });

  for (int i = 0; i < ship_id_initiative.size(); ++i) {
    auto id = std::get<0>(ship_id_initiative[i]);
    initiative_order.turnorder[i] = id;
  }
  return initiative_order;
}

}// namespace esd