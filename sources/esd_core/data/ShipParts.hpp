//
// Created by aypahyo on 15.12.22.
//

#ifndef SOURCES_ESD_CORE_DATA_SHIPPARTS_HPP_
#define SOURCES_ESD_CORE_DATA_SHIPPARTS_HPP_

#include <esd_core/data/ShipFightStats.hpp>

namespace esd {
//default
void default_hull (esd::ShipFightStats *s);
void default_nuclear_drive(esd::ShipFightStats *s);
void default_nuclear_source(esd::ShipFightStats *s);
void default_ion_cannon(esd::ShipFightStats *s);
void default_electron_computer(esd::ShipFightStats *s);
//military
void military_plasma_cannon(esd::ShipFightStats *s);
void military_phase_shield(esd::ShipFightStats *s);
void military_tachyon_source(esd::ShipFightStats *s);
void military_gluon_computer(esd::ShipFightStats *s);
void military_plasma_missile(esd::ShipFightStats *s);
//grid
void grid_gauss_shield(esd::ShipFightStats *s);
void grid_fusion_source(esd::ShipFightStats *s);
void grid_improved_hull(esd::ShipFightStats *s);
void grid_positron_computer(esd::ShipFightStats *s);
void grid_tachyon_drive(esd::ShipFightStats *s);
void grid_antimatter_cannon(esd::ShipFightStats *s);
//nano
void nano_fusion_drive(esd::ShipFightStats *s);
//rare
void rare_conifold_field(esd::ShipFightStats *s);
void rare_absorption_shield(esd::ShipFightStats *s);
void rare_sentient_hull(esd::ShipFightStats *s);
void rare_soliton_cannon(esd::ShipFightStats *s);
void rare_transition_drive(esd::ShipFightStats *s);
void rare_flux_missile(esd::ShipFightStats *s);
void rare_zero_point_source(esd::ShipFightStats *s);
//ancient
void ancient_ion_disruptor(esd::ShipFightStats *s);
void ancient_ion_turret(esd::ShipFightStats *s);
void ancient_plasma_turret(esd::ShipFightStats *s);
void ancient_soliton_charger(esd::ShipFightStats *s);
void ancient_ion_missile(esd::ShipFightStats *s);
void ancient_axionen_computer(esd::ShipFightStats *s);
void ancient_antimatter_missile(esd::ShipFightStats *s);
void ancient_flux_shield(esd::ShipFightStats *s);
void ancient_conformal_drive(esd::ShipFightStats *s);
void ancient_nonlinear_drive(esd::ShipFightStats *s);
void ancient_shard_hull(esd::ShipFightStats *s);
void ancient_hypergrid_source(esd::ShipFightStats *s);
void ancient_inversion_shield(esd::ShipFightStats *s);
void ancient_soliton_missile(esd::ShipFightStats *s);
void ancient_muon_source(esd::ShipFightStats *s);
//blueprint
void blueprint_energy_0(esd::ShipFightStats *s);
void blueprint_energy_1(esd::ShipFightStats *s);
void blueprint_energy_2(esd::ShipFightStats *s);
void blueprint_energy_3(esd::ShipFightStats *s);
void blueprint_energy_5(esd::ShipFightStats *s);
void blueprint_initiative_0 (esd::ShipFightStats *s);
void blueprint_initiative_1(esd::ShipFightStats *s);
void blueprint_initiative_2(esd::ShipFightStats *s);
void blueprint_initiative_3(esd::ShipFightStats *s);
void blueprint_initiative_4(esd::ShipFightStats *s);
void blueprint_initiative_5(esd::ShipFightStats *s);
void blueprint_plantputer_a(esd::ShipFightStats *s);
void blueprint_plantputer_b(esd::ShipFightStats *s);
}// namespace esd

#endif//SOURCES_ESD_CORE_DATA_SHIPPARTS_HPP_
