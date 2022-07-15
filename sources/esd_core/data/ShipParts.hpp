//
// Created by aypahyo on 09.08.21.
//

#include <esd_core/data/ShipPart.hpp>

#ifndef SOURCES_ESD_CORE_DATA_SHIPPARTS_HPP_
#define SOURCES_ESD_CORE_DATA_SHIPPARTS_HPP_

namespace esd {
//default
constexpr ShipPart default_hull = [](ShipPart p) { p.hulls = 1; p.space=1; return p; }(ShipPart{});
constexpr ShipPart default_nuclear_drive = [](ShipPart p) { p.drives = 1; p.energy = -1; p.initiative = 1; p.space=1; return p; }(ShipPart{});
constexpr ShipPart default_nuclear_source = [](ShipPart p) { p.energy = 3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart default_ion_cannon = [](ShipPart p) { p.yellow_cannon = 1; p.energy = -1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart default_electron_computer = [](ShipPart p) { p.computers = 1; p.space = 1; return p; }(ShipPart{});
//military
constexpr ShipPart military_plasma_cannon = [](ShipPart p) { p.orange_cannon = 1; p.energy = -2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart military_phase_shield = [](ShipPart p) { p.shields = 2; p.energy = -1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart military_tachyon_source = [](ShipPart p) { p.energy = 9; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart military_gluon_computer = [](ShipPart p) { p.computers = 3; p.energy = -2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart military_plasma_missile = [](ShipPart p) { p.orange_rocket = 2; p.energy = -1; p.space = 1; return p; }(ShipPart{});
//grid
constexpr ShipPart grid_gauss_shield = [](ShipPart p) { p.shields = 1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart grid_fusion_source = [](ShipPart p) { p.energy = 6; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart grid_improved_hull = [](ShipPart p) { p.hulls = 2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart grid_positron_computer = [](ShipPart p) { p.computers = 2; p.energy = -1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart grid_tachyon_drive = [](ShipPart p) { p.drives = 3; p.initiative = 3; p.energy = -3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart grid_antimatter_cannon = [](ShipPart p) { p.red_cannon = 1; p.energy = -4; p.space = 1; return p; }(ShipPart{});
//nano
constexpr ShipPart nano_fusion_drive = [](ShipPart p) { p.drives = 2; p.initiative = 2; p.energy = -2; p.space = 1; return p; }(ShipPart{});
//rare
constexpr ShipPart rare_conifold_field = [](ShipPart p) { p.hulls = 3; p.energy = -2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart rare_absorption_shield = [](ShipPart p) { p.shields = 1; p.energy = 4; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart rare_sentient_hull = [](ShipPart p) { p.computers = 1; p.hulls = 1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart rare_soliton_cannon = [](ShipPart p) { p.blue_cannon = 1; p.energy = -3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart rare_transition_drive = [](ShipPart p) { p.drives = 3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart rare_flux_missile = [](ShipPart p) { p.yellow_rocket = 2; p.initiative = 1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart rare_zero_point_source = [](ShipPart p) { p.energy = 12; p.space = 1; return p; }(ShipPart{});
//ancient
constexpr ShipPart ancient_ion_disruptor = [](ShipPart p) { p.yellow_cannon = 1; p.initiative = 3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_ion_turret = [](ShipPart p) { p.yellow_cannon = 2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_plasma_turret = [](ShipPart p) { p.orange_cannon = 2; p.energy =-3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_soliton_charger = [](ShipPart p) { p.blue_cannon = 1; p.energy =-1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_ion_missile = [](ShipPart p) { p.yellow_rocket = 3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_axionen_computer = [](ShipPart p) { p.computers = 2; p.initiative = 1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_antimatter_missile = [](ShipPart p) { p.red_rocket = 1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_flux_shield = [](ShipPart p) { p.shields = 3; p.initiative = 1; p.energy = -2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_conformal_drive = [](ShipPart p) { p.drives = 4; p.initiative = 2; p.energy = -2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_nonlinear_drive = [](ShipPart p) { p.drives = 2; p.energy = 2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_shard_hull = [](ShipPart p) { p.hulls = 3; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_hypergrid_source = [](ShipPart p) { p.energy = 11; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_inversion_shield = [](ShipPart p) { p.shields = 2; p.energy = 2; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_soliton_missile = [](ShipPart p) { p.blue_rocket = 1; p.initiative = 1; p.space = 1; return p; }(ShipPart{});
constexpr ShipPart ancient_muon_source = [](ShipPart p) { p.energy = 2; return p; }(ShipPart{});
//blueprint
constexpr ShipPart blueprint_energy_0 = ShipPart{};
constexpr ShipPart blueprint_energy_1 = [](ShipPart p) { p.energy = 1; return p; }(ShipPart{});
constexpr ShipPart blueprint_energy_2 = [](ShipPart p) { p.energy = 2; return p; }(ShipPart{});
constexpr ShipPart blueprint_energy_3 = [](ShipPart p) { p.energy = 3; return p; }(ShipPart{});
constexpr ShipPart blueprint_energy_5 = [](ShipPart p) { p.energy = 4; return p; }(ShipPart{});
constexpr ShipPart blueprint_initiative_0 = ShipPart{};
constexpr ShipPart blueprint_initiative_1 = [](ShipPart p) { p.initiative = 1; return p; }(ShipPart{});
constexpr ShipPart blueprint_initiative_2 = [](ShipPart p) { p.initiative = 2; return p; }(ShipPart{});
constexpr ShipPart blueprint_initiative_3 = [](ShipPart p) { p.initiative = 3; return p; }(ShipPart{});
constexpr ShipPart blueprint_initiative_4 = [](ShipPart p) { p.initiative = 4; return p; }(ShipPart{});
constexpr ShipPart blueprint_initiative_5 = [](ShipPart p) { p.initiative = 5; return p; }(ShipPart{});
constexpr ShipPart blueprint_plantputer_a = [](ShipPart p) { p.computers = 1; return p; }(ShipPart{});
constexpr ShipPart blueprint_plantputer_b = [](ShipPart p) { p.computers = 1; p.initiative = 2; return p; }(ShipPart{});
}// namespace esd
#endif//SOURCES_ESD_CORE_DATA_SHIPPARTS_HPP_
