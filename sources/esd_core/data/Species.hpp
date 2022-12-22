//
// Created by aypahyo on 09.08.21.
//

#ifndef SOURCES_ESD_CORE_DATA_SPECIES_HPP_
#define SOURCES_ESD_CORE_DATA_SPECIES_HPP_

//#include <esd_core/data/Specie.hpp>
//#include <esd_core/data/ShipParts.hpp>
//
//namespace esd {
//
//Specie human = [](Specie s) {
//  s.interceptor_energy = blueprint_energy_0;
//  s.interceptor_other = blueprint_initiative_2;
//  s.interceptor_space_min = 3;
//  s.interceptor_space_max = 4;
//  s.cruiser_energy = blueprint_energy_0;
//  s.cruiser_other = blueprint_initiative_1;
//  s.cruiser_space_min = 5;
//  s.cruiser_space_max = 6;
//  s.dreadnought_energy = blueprint_energy_0;
//  s.dreadnought_other = blueprint_initiative_0;
//  s.dreadnought_space_min = 7;
//  s.dreadnought_space_max = 8;
//  s.starbase_energy = blueprint_energy_3;
//  s.starbase_other = blueprint_initiative_4;
//  s.starbase_space_min = 4;
//  s.starbase_space_max = 5;
//  s.default_Tiles_count = 0;
//  s.default_Tiles[s.default_Tiles_count++] = default_hull;
//  s.default_Tiles[s.default_Tiles_count++] = default_nuclear_drive;
//  s.default_Tiles[s.default_Tiles_count++] = default_nuclear_source;
//  s.default_Tiles[s.default_Tiles_count++] = default_ion_cannon;
//  s.default_Tiles[s.default_Tiles_count++] = default_electron_computer;
//  return s; }(Specie{});
//
//Specie eridani_empire = [](Specie s) {
//  s.interceptor_energy = blueprint_energy_1;
//  s.cruiser_energy = blueprint_energy_1;
//  s.dreadnought_energy = blueprint_energy_1;
//  s.default_Tiles[s.default_Tiles_count++] = nano_fusion_drive;
//  s.default_Tiles[s.default_Tiles_count++] = grid_gauss_shield;
//  s.default_Tiles[s.default_Tiles_count++] = military_plasma_cannon;
//  return s; }(human);
//
//Specie hydran_progress = human;
//
//Specie planta = [](Specie s) {
//  s.interceptor_energy = blueprint_energy_2;
//  s.interceptor_other = blueprint_plantputer_a;
//  s.interceptor_space_min = 3;
//  s.interceptor_space_max = 3;
//  s.cruiser_energy = blueprint_energy_2;
//  s.cruiser_other = blueprint_plantputer_a;
//  s.cruiser_space_min = 4;
//  s.cruiser_space_max = 5;
//  s.dreadnought_energy = blueprint_energy_2;
//  s.dreadnought_other = blueprint_plantputer_a;
//  s.dreadnought_space_min = 6;
//  s.dreadnought_space_max = 7;
//  s.starbase_energy = blueprint_energy_5;
//  s.starbase_other = blueprint_plantputer_b;
//  s.starbase_space_min = 4;
//  s.starbase_space_max = 4;
//  return s; }(human);
//
//
//Specie decendnts_of_draco = [](Specie s) {
//  s.default_Tiles[s.default_Tiles_count++] = nano_fusion_drive;
//  return s; }(human);
//
//Specie mechanema = [](Specie s) {
//  s.default_Tiles[s.default_Tiles_count++] = grid_positron_computer;
//  return s; }(human);
//
//Specie orion_hegemony = [](Specie s) {
//  s.interceptor_energy = blueprint_energy_1;
//  s.interceptor_other = blueprint_initiative_3;
//  s.interceptor_space_min = 4;
//  s.interceptor_space_max = 4;
//  s.cruiser_energy = blueprint_energy_2;
//  s.cruiser_other = blueprint_initiative_2;
//  s.cruiser_space_min = 6;
//  s.cruiser_space_max = 6;
//  s.dreadnought_energy = blueprint_energy_3;
//  s.dreadnought_other = blueprint_initiative_1;
//  s.dreadnought_space_min = 8;
//  s.dreadnought_space_max = 8;
//  s.starbase_other = blueprint_initiative_5;
//  s.starbase_space_min = 5;
//  s.starbase_space_max = 5;
//  s.default_Tiles[s.default_Tiles_count++] = grid_gauss_shield;
//  return s; }(human);
//
//Specie terran_directorate = human;
//Specie terran_federation = human;
//Specie terran_union = human;
//Specie terran_republic = human;
//Specie terran_conglomerate = human;
//Specie terran_alliance = human;
//
//}

#endif//SOURCES_ESD_CORE_DATA_SPECIES_HPP_
