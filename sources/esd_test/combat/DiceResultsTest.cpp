//
// Created by aypahyo on 10.08.21.
//
#include <esd_core/combat/CreateResults.hpp>
#include <gtest/gtest.h>

TEST(diceresults, dice_1d6_yellow) {
  auto results = esd::CreateResults::GetAllRolls(1,0,0,0);
  ASSERT_EQ(results.size(), 6);
  ASSERT_EQ(results[0].yellow_results(0), 1);
  ASSERT_EQ(results[1].yellow_results(0), 2);
  ASSERT_EQ(results[2].yellow_results(0), 3);
  ASSERT_EQ(results[3].yellow_results(0), 4);
  ASSERT_EQ(results[4].yellow_results(0), 5);
  ASSERT_EQ(results[5].yellow_results(0), 6);
}

TEST(diceresults, dice_2d6_yellow) {
  auto results = esd::CreateResults::GetAllRolls(2,0,0,0);
  ASSERT_EQ(results.size(), 36);
  ASSERT_EQ(results[0].yellow_results(0), 1);
  ASSERT_EQ(results[0].yellow_results(1), 1);
  ASSERT_EQ(results[35].yellow_results(0), 6);
  ASSERT_EQ(results[35].yellow_results(1), 6);
}

TEST(diceresults, dice_1d6y_1d6o) {
  auto results = esd::CreateResults::GetAllRolls(1,1,0,0);
  ASSERT_EQ(results.size(), 36);
  ASSERT_EQ(results[0].yellow_results(0), 1);
  ASSERT_EQ(results[0].orange_results(0), 1);
  ASSERT_EQ(results[35].yellow_results(0), 6);
  ASSERT_EQ(results[35].orange_results(0), 6);
}

TEST(diceresults, dice_1d6y_1d6b) {
  auto results = esd::CreateResults::GetAllRolls(1,0,1,0);
  ASSERT_EQ(results.size(), 36);
  ASSERT_EQ(results[0].yellow_results(0), 1);
  ASSERT_EQ(results[0].blue_results(0), 1);
  ASSERT_EQ(results[35].yellow_results(0), 6);
  ASSERT_EQ(results[35].blue_results(0), 6);
}

TEST(diceresults, dice_1d6y_1d6r) {
  auto results = esd::CreateResults::GetAllRolls(1,0,0,1);
  ASSERT_EQ(results.size(), 36);
  ASSERT_EQ(results[0].yellow_results(0), 1);
  ASSERT_EQ(results[0].red_results(0), 1);
  ASSERT_EQ(results[35].yellow_results(0), 6);
  ASSERT_EQ(results[35].red_results(0), 6);
}

TEST(diceresults, dice_16d6_yellow) {
  //fifteen yellow dice is the maximum in a dreadnought class
  constexpr auto number_6_pow_15 = 470184984576;
  constexpr auto first_index = 0;
  constexpr auto last_index = number_6_pow_15 - 1;
  auto first_result = esd::CreateResults::getOutcome(15, 0,0,0, first_index);
  auto last_result = esd::CreateResults::getOutcome(15, 0,0,0, last_index);

  for(int i = 0; i < 15; ++i) ASSERT_EQ(first_result.yellow_results(i), 1);
  for(int i = 0; i < 15; ++i) ASSERT_EQ(last_result.yellow_results(i), 6);
}

TEST(diceresults, dice_2d6_yellow_single_generation) {
  for(int a = 0; a < 6; ++a)
  for(int b = 0; b < 6; ++b)
  {
    auto seq = a * 6 + b;
    auto results = esd::CreateResults::getOutcome(2,0,0,0, seq);
    ASSERT_EQ(results.yellow_results(1), a+1);
    ASSERT_EQ(results.yellow_results(0), b+1);
  }
}