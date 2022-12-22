//
// Created by aypahyo on 14.12.22.
//
#include <esd_core/data/ShipFightStats.hpp>
#include <gtest/gtest.h>

TEST(shiptype, has_attributes) {
  esd::ShipFightStats uut{};

  ASSERT_EQ(0, uut.initiative);
  ASSERT_EQ(0, uut.hulls);
  ASSERT_EQ(0, uut.computers);
  ASSERT_EQ(0, uut.shields);
  ASSERT_EQ(0, uut.cannon_yellow);
  ASSERT_EQ(0, uut.cannon_red);




}
