//
// Created by aypahyo on 15.12.22.
//

#include "ShipParts.hpp"

namespace esd {
//default
void default_hull (esd::ShipFightStats *s) { s->hulls+=1; }
void default_nuclear_drive(esd::ShipFightStats *s) { s->initiative += 1; }
void default_nuclear_source(esd::ShipFightStats *s) {  }
void default_ion_cannon(esd::ShipFightStats *s) { s->cannon_yellow += 1; }
void default_electron_computer(esd::ShipFightStats *s) { s->computers += 1; }
//military
void military_plasma_cannon(esd::ShipFightStats *s) { s->cannon_orange += 1; }
void military_phase_shield(esd::ShipFightStats *s) { s->shields += 2; }
void military_tachyon_source(esd::ShipFightStats *s) { }
void military_gluon_computer(esd::ShipFightStats *s) { s->computers += 3; }
void military_plasma_missile(esd::ShipFightStats *s) { s->rocket_orange += 2;  }
//grid
void grid_gauss_shield(esd::ShipFightStats *s) { s->shields += 1;  }
void grid_fusion_source(esd::ShipFightStats *s) {  }
void grid_improved_hull(esd::ShipFightStats *s) { s->hulls += 2; }
void grid_positron_computer(esd::ShipFightStats *s) { s->computers += 2; }
void grid_tachyon_drive(esd::ShipFightStats *s) { s->initiative += 3; }
void grid_antimatter_cannon(esd::ShipFightStats *s) { s->cannon_red += 1;  }
//nano
void nano_fusion_drive(esd::ShipFightStats *s) { s->initiative += 2;  }
//rare
void rare_conifold_field(esd::ShipFightStats *s) { s->hulls += 3; }
void rare_absorption_shield(esd::ShipFightStats *s) { s->shields += 1; }
void rare_sentient_hull(esd::ShipFightStats *s) { s->computers += 1; s->hulls += 1; }
void rare_soliton_cannon(esd::ShipFightStats *s) { s->cannon_blue += 1; }
void rare_transition_drive(esd::ShipFightStats *s) {  }
void rare_flux_missile(esd::ShipFightStats *s) { s->rocket_yellow += 2; s->initiative += 1; }
void rare_zero_point_source(esd::ShipFightStats *s) {  }
//ancient
void ancient_ion_disruptor(esd::ShipFightStats *s) { s->cannon_yellow += 1; s->initiative += 3; }
void ancient_ion_turret(esd::ShipFightStats *s) { s->cannon_yellow += 2; }
void ancient_plasma_turret(esd::ShipFightStats *s) { s->cannon_orange += 2;  }
void ancient_soliton_charger(esd::ShipFightStats *s) { s->cannon_blue += 1; }
void ancient_ion_missile(esd::ShipFightStats *s) { s->rocket_yellow += 3;  }
void ancient_axionen_computer(esd::ShipFightStats *s) { s->computers += 2; s->initiative += 1; }
void ancient_antimatter_missile(esd::ShipFightStats *s) { s->rocket_red += 1; }
void ancient_flux_shield(esd::ShipFightStats *s) { s->shields += 3; s->initiative += 1; }
void ancient_conformal_drive(esd::ShipFightStats *s) { s->initiative += 2; }
void ancient_nonlinear_drive(esd::ShipFightStats *s) { }
void ancient_shard_hull(esd::ShipFightStats *s) { s->hulls += 3; }
void ancient_hypergrid_source(esd::ShipFightStats *s) { }
void ancient_inversion_shield(esd::ShipFightStats *s) { s->shields += 2; }
void ancient_soliton_missile(esd::ShipFightStats *s) { s->rocket_blue += 1; s->initiative += 1; }
void ancient_muon_source(esd::ShipFightStats *s) { }
//blueprint
void blueprint_energy_0(esd::ShipFightStats *s) { }
void blueprint_energy_1(esd::ShipFightStats *s) { }
void blueprint_energy_2(esd::ShipFightStats *s) { }
void blueprint_energy_3(esd::ShipFightStats *s) { }
void blueprint_energy_5(esd::ShipFightStats *s) { }
void blueprint_initiative_0 (esd::ShipFightStats *s) { }
void blueprint_initiative_1(esd::ShipFightStats *s) { s->initiative += 1; }
void blueprint_initiative_2(esd::ShipFightStats *s) { s->initiative += 2; }
void blueprint_initiative_3(esd::ShipFightStats *s) { s->initiative += 3; }
void blueprint_initiative_4(esd::ShipFightStats *s) { s->initiative += 4; }
void blueprint_initiative_5(esd::ShipFightStats *s) { s->initiative += 5; }
void blueprint_plantputer_a(esd::ShipFightStats *s) { s->computers += 1; }
void blueprint_plantputer_b(esd::ShipFightStats *s) { s->computers += 1; s->initiative += 2; }
}// namespace esd