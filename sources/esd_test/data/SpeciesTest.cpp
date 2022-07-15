//
// Created by aypahyo on 09.08.21.
//
#include <gtest/gtest.h>
#include <esd_core/data/Species.hpp>

TEST(species_data, content_test_tiles_count) {
  using namespace esd;
  ASSERT_EQ(human.default_Tiles_count, 5);
  ASSERT_EQ(eridani_empire.default_Tiles_count, 8);
  ASSERT_EQ(hydran_progress.default_Tiles_count, 5);
  ASSERT_EQ(planta.default_Tiles_count, 5);
  ASSERT_EQ(decendants_of_draco.default_Tiles_count, 6);
  ASSERT_EQ(mechanema.default_Tiles_count, 6);
  ASSERT_EQ(orion_hegemony.default_Tiles_count, 6);
  ASSERT_EQ(terran_federation.default_Tiles_count, 5);
  ASSERT_EQ(terran_union.default_Tiles_count, 5);
  ASSERT_EQ(terran_republic.default_Tiles_count, 5);
  ASSERT_EQ(terran_conglomerate.default_Tiles_count, 5);
  ASSERT_EQ(terran_directorate.default_Tiles_count, 5);
  ASSERT_EQ(terran_alliance.default_Tiles_count, 5);
}

TEST(species_data, content_test_dreadnought_energy) {
  using namespace esd;
  ASSERT_EQ(human.dreadnought_energy.energy, 0);
  ASSERT_EQ(eridani_empire.dreadnought_energy.energy, 1);
  ASSERT_EQ(hydran_progress.dreadnought_energy.energy, 0);
  ASSERT_EQ(planta.dreadnought_energy.energy, 2);
  ASSERT_EQ(decendants_of_draco.dreadnought_energy.energy, 0);
  ASSERT_EQ(mechanema.dreadnought_energy.energy, 0);
  ASSERT_EQ(orion_hegemony.dreadnought_energy.energy, 3);
  ASSERT_EQ(terran_federation.dreadnought_energy.energy, 0);
  ASSERT_EQ(terran_union.dreadnought_energy.energy, 0);
  ASSERT_EQ(terran_republic.dreadnought_energy.energy, 0);
  ASSERT_EQ(terran_conglomerate.dreadnought_energy.energy, 0);
  ASSERT_EQ(terran_directorate.dreadnought_energy.energy, 0);
  ASSERT_EQ(terran_alliance.dreadnought_energy.energy, 0);
}