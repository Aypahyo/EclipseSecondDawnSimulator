//
// Created by aypahyo on 09.08.21.
//
#include <esd_core/data/ShipBuilder.hpp>
#include <esd_core/data/ShipParts.hpp>
#include <gtest/gtest.h>

TEST(ship_parts_data, content_test) {

  using namespace esd;
  ASSERT_EQ(esd::ShipBuilder{}.w(default_hull).stats().hulls, 1);
  //ASSERT_EQ(esd::ShipBuilder{}.w(default_nuclear_drive).stats().drives, 1);
  //ASSERT_EQ(esd::ShipBuilder{}.w(default_nuclear_drive).stats().energy, -1);
  ASSERT_EQ(esd::ShipBuilder{}.w(default_nuclear_drive).stats().initiative, 1);
  //ASSERT_EQ(esd::ShipBuilder{}.w(default_nuclear_source).stats().energy, 3);
  ASSERT_EQ(esd::ShipBuilder{}.w(default_ion_cannon).stats().cannon_yellow, 1);
  ASSERT_EQ(esd::ShipBuilder{}.w(military_plasma_cannon).stats().cannon_orange, 1);
  ASSERT_EQ(esd::ShipBuilder{}.w(rare_soliton_cannon).stats().cannon_blue, 1);
  ASSERT_EQ(esd::ShipBuilder{}.w(grid_antimatter_cannon).stats().cannon_red, 1);
}
