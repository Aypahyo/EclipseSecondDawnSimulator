# Eclipse: Second Dawn Space Combat Simulator Specification

> Last updated: 2025-11-21

## 1. Purpose & Scope

This document defines the product and technical requirements for the space-combat simulator that powers `BattleSimulator::simulate` in EclipseSecondDawnSimulator. The specification is grounded in the expert reference "Eclipse: Second Dawn for the Galaxy — Space Combat & Ship Parts" and focuses on rules, data modeling, and algorithms needed to faithfully reproduce tabletop outcomes. The simulator must:

- Resolve combats between any two factions (players, NPCs, Ancients/Guardians) that share a sector.
- Support all standard ship classes, parts from technologies, and discovery tiles that influence combat.
- Enforce fleet-size / ship-count limitations when building states.
- Produce repeatable probabilistic summaries (win/draw odds, expected rounds) suitable for UI display and AI inputs.

Out of scope: ground combat, influence/movement rules, non-combat discovery tiles, diplomacy, upkeep, and AI decision logic.

## 2. References

- **Primary rules digest**: _Eclipse: Second Dawn for the Galaxy — Space Combat & Ship Parts Reference_ (consultant-provided markdown).
- **Source code**: `src/game/battle_simulator.cpp`, `include/game/battle_simulator.hpp`, `include/game/types.hpp` (ship loadouts and derived stats).

## 3. Functional Requirements

### 3.1 Combat Flow

1. **Input fleets**: Every valid `ShipLoadout` in the provided vectors must be converted into an internal `BattleShipProfile` (hull, dice, computers, shields, initiative buckets, missile payload).
2. **Pre-combat effects**: Resolve one-shot missile systems (tech or discovery) before Round 1. Missiles obey initiative 0 and never repeat.
3. **Round loop**:
   - Compute initiative buckets (higher value fires earlier). Weapons with identical initiative are simultaneous.
   - For each bucket: attackers roll dice for every surviving weapon, applying computer vs shield modifiers to determine hit thresholds.
   - Apply hits immediately within the bucket. Destroyed ships do not fire in later buckets or later rounds.
4. **Damage allocation**: Side owning damaged ships chooses assignment; simulator must implement deterministic heuristics (e.g., sacrifice lowest initiative/lowest remaining hull) to avoid random swings unless randomness is explicitly desired.
5. **Termination**: Combat ends when all ships of a side (excluding optional retreats) are destroyed. Optional retreat logic is deferred (documented in §9.3).
6. **Outcome**: Return `BattleSummary { humanWin, alienWin, draw, expectedRounds }` such that probabilities sum to 1. Draws arise from stalemates (no progress possible).

### 3.2 Fleet & Ship Constraints

- Obey maximum ship counts per empire (Interceptors 8, Cruisers 4, Dreadnoughts 2, Starbases 1/sector unlimited overall). Reject or truncate invalid inputs.
- Fleet tiles limit number of fleets per sector (default 2 → up to 5). Simulator accepts only legal sector compositions.
- Starbases never require a fleet tile and may co-exist with ships exceeding fleet-tile capacity, but only one starbase per sector.

### 3.3 Parts Coverage

The data layer must encode every combat-relevant part listed in the reference:

- **Weapons**: Ion, Plasma, Positron, Antimatter cannons; Nuclear & Plasma missiles; Ancient Cannon/Ion Turret/Missile.
- **Computer bonuses**: Electron (+1), Positron (+2), Gluon (+3), Ancient (+2).
- **Shields**: Gauss (+1), Phase (+2), Advanced (+3), Ancient (+2).
- **Hull & Support**: +1 Hull, Improved Hull (+2), Ancient Hull (+1), Flux Shield (prevent 1 damage/round), Quantum Grid (energy budget), Improved Reactor (+1 energy).
- **Movement-only parts** (Ion Thruster) documented but not directly used here except for validation (ignored in combat math).

Each part definition requires:

- Unique identifier
- Slot compatibility (weapon/computer/shield/hull/support/missile)
- Numeric effects (dice count/type, hit threshold modifiers, initiative, one-shot flag)


## 4. Data Model

| Entity | Description | Key Fields |
| --- | --- | --- |
| `ShipLoadout` | Existing higher-level type describing a ship plus installed parts. | `hullParts`, `weaponParts`, `computerParts`, `shieldParts`, `supportParts`, `energyBudget` |
| `BattleShipProfile` | Runtime, combat-only description of a ship. | `baseHull`, `currentHull`, `weapons[]`, `missiles[]`, `computerBonus`, `shieldBonus`, `fluxShieldCharges` |
| `WeaponInstance` | Resolved weapon ready to roll. | `diceCount`, `dieType (D6/D8)`, `baseToHit`, `initiative`, `isMissile`, `isOneShot`, `sourceId` |
| `BattleState` | Sorted vectors of `BattleShipProfile` for each faction plus metadata for memoization. | `humans`, `aliens`, `hash` |
| `CachedResult` | Memoized recursion output. | `humanWin`, `alienWin`, `draw`, `expectedRounds` |

### Derived Stats

- **Total computer bonus** = sum of installed computers (capped if rules limit stacking; default uncapped) + base ship computer.
- **Total shield bonus** = base + installed shields.
- **Hull** = base hull + hull part bonuses.
- **Initiative buckets** = per-weapon value; starbases inherit part initiatives.
- **Energy validation** = ensure installed parts do not exceed ship energy (base + reactors + quantum grid). Failing ships are excluded from combat.

## 5. Algorithms

1. **State canonicalization**: Sort ships on each side by descending `battleCompare` (hull, then dice, computers, shields) to ensure deterministic hashing.
2. **Hit distribution**: For each side, build per-bucket hit-count probability mass functions using binomial distributions (`binomialDistribution(dice, success)`), convolving across ships.
3. **Damage resolution**: Convert probabilistic hits into recursive exploration of resulting states (as currently implemented). Apply hits in order of owner preference (default: lowest hull first, tie-breaking by lowest initiative and then lowest computer).
4. **Flux shield handling**: When a ship with `fluxShieldCharges > 0` receives hits within a round, prevent the first point of damage and decrement the charge.
5. **Missile phase**: Evaluate missile weapons before main initiative loop; remove `isOneShot` weapons after firing regardless of hit.
6. **Memoization**: Use unordered_map keyed by `BattleState` (requires `BattleShipProfile::operator==` and `StateHash`).
7. **Stalemate detection**: If `progressProbability` is zero (no state change possible), return draw per rules reference §Common Clarifications. Document optional cutoff for extremely long loops (e.g., bail after 200 rounds, treat as draw).

## 6. Edge Cases & Clarifications

- **Damage does not spill**: Each hit targets one ship; excess over hull is lost.
- **Simultaneous fire**: Weapons with identical initiative trade damage simultaneously; remove destroyed ships only after processing the entire bucket.
- **NPC target choice**: Default heuristic is uniform random valid target. Provide hook for deterministic mode (seeded RNG) for reproducibility.
- **Retreats**: Not implemented yet; placeholder API should allow future `simulate(..., allowRetreats=true)` parameter.
- **Starbases**: Always defend; ignore fleet tiles; treated as ships with fixed loadout (initiative 4 base weapon + installed parts if tech allows?).
- **Discovery-only parts**: Mark as unique sources but treat identically to their tech counterparts after acquisition.

## 7. Validation & Testing Strategy

- **Unit tests**: Cover individual weapon probability calculations, damage allocation heuristics, missile phase behavior, flux shield prevention, and stalemate classification.
- **Regression cases**: Encode handbook examples (initiative interaction, computer vs shield offsets, missile-only exchanges) as deterministic scenarios with expected probability outputs.
- **Monte Carlo cross-check**: (Optional) Provide brute-force simulator to sample combats and compare with analytical results for sanity.
- **Data integrity tests**: Ensure part catalog includes every item listed in §3.3, with correct stats and slot allowances.

## 8. Performance & Determinism

- Target completions within <20 ms for typical fleet sizes (≤14 ships per side) on modern CPUs.
- Memoization table should use structural hashing to avoid exponential blow-up.
- RNG (if used for heuristics) must be seedable via simulator configuration for reproducible tests.

## 9. Future Enhancements / Open Questions

1. **Retreat rules**: Add API and state transitions for players withdrawing after missile phase or later rounds, respecting initiative locks.
2. **Ancient allies**: Discovery tile granting an Ancient cruiser to the player for one combat—needs representation in `ShipLoadout` import pipeline.
3. **UI hooks**: Provide detailed combat logs (per-round, per-initiative) for debugging and educational overlays.
4. **Multiple factions**: Current engine assumes two sides; document steps for extending to three-way combats if expansions require it.

## 10. Deliverables

- Updated data catalogs (JSON or constexpr tables) for ship parts, with unit tests verifying completeness.
- Implementation updates inside `src/game/battle_simulator.cpp` adhering to this spec.
- Developer documentation describing how to add new parts or clarify rules additions.

## 11. Ship Blueprint & Upgrade Reference

The simulator shares its data model with the blueprint/upgrade UI. This section captures the authoritative consulting notes so engineering can keep both systems aligned.

### 11.1 Blueprint Structure

Every faction controls four ship blueprints—**Interceptor**, **Cruiser**, **Dreadnought**, and **Starbase**. Each blueprint contains:

1. **Ship Part Spaces**: A grid of generic slots with no per-slot type restriction. Any tile can technically occupy any space, subject only to the global placement constraints below.
2. **Pre-Printed Parts ("preprints")**: Symbols drawn in specific slots on the cardboard blueprint. They function as permanent bottom-layer parts that can be overridden by tiles but never removed from the stack.
3. **Blueprint Extras**: Optional symbols printed outside the grid. They behave exactly like parts, but cannot be removed or covered and are always active.
4. **Blueprint Properties**: Base initiative bonuses, baked-in energy production from extras, and faction modifiers (e.g., +energy, +computer).

### 11.2 Stack & Overlay Mechanics

Each grid slot is a tiny stack:

```text
Top    → Player tile, if one is installed
Bottom → Printed symbol, if one exists
```

- When a tile is present, only the tile's stats apply.
- Removing the tile reactivates the preprint immediately.
- Empty slots with no preprint contribute nothing.
- Extras outside the grid are immutable and always count.

### 11.3 Placement Restrictions (Rules Enforcement)

- **Energy**: `Σ energyConsumption ≤ Σ energyProduction` must hold for the entire blueprint. Reject any placement violating this budget.
- **Drive Requirement**: Interceptors, Cruisers, and Dreadnoughts must end with at least one active drive (preprint or tile). Starbases may never contain drives.
- **Tech Ownership**: A tile can be placed only if its technology is researched (unless a discovery explicitly overrides this).
- **Ancient Tile Removal**: Removing an Ancient part from a ship removes that tile from the game permanently.
- **Starbase Drive Ban**: Any attempt to place a drive tile on a starbase blueprint is illegal.

### 11.4 Human (Terran) Baseline Preprints

The Terran layouts define the canonical slot counts (Interceptor 4, Cruiser 6, Dreadnought 8, Starbase 5) and preprints that other factions modify:

- **Interceptor**: Ion Cannon, Nuclear Drive, Basic Reactor (+3 energy), one free slot; base initiative +2.
- **Cruiser**: Ion Cannon, Nuclear Drive, Basic Reactor, Computer +1, Hull +1, one free slot; base initiative +1.
- **Dreadnought**: Two Ion Cannons, two Hull +1, Computer +1, Nuclear Drive, Basic Reactor, one free slot; base initiative +0.
- **Starbase**: Computer +1, Ion Cannon, two Hull +1, one free slot, immutable Basic Reactor (+3 energy) as an extra; base initiative +5 (no drives allowed).

### 11.5 Faction Modifiers

- **Eridani Empire**: Identical layout to Humans, but each non-starbase blueprint gains +1 energy production (starbase unchanged).
- **PLANTA**: Layout swaps one printed slot to a free `X` slot per blueprint and adds +1 computer/+1 energy (Interceptor, Cruiser, Dreadnought) or +1 computer/+5 energy (Starbase). All base initiatives become 0 except the starbase, which is 2. Layout diagrams remain the same size as Terran boards.
- **Orion Hegemony**: Human layout. Initiative bonuses change to 3/2/1/5 for Interceptor/Cruiser/Dreadnought/Starbase respectively. Non-starbase blueprints gain +1/+2/+3 energy. Every free slot starts with a preprinted "-1 shield" Gauss Shield that can be overridden by tiles.

### 11.6 Derived Stat Computation

Final ship stats are determined by summing:

1. Active tiles (top of stack in each slot)
2. Uncovered preprints (bottom layer when no tile is installed)
3. Extras outside the grid
4. Faction-wide modifiers (energy, computers, initiative, etc.)

The totals drive `ShipLoadout::derivedStats()` (energy produced vs consumed, initiative, drives/movement, weapon dice by type, hull, shields, computers). The existing energy validation should incorporate blueprint extras and faction modifiers before allowing loadouts into combat.

### 11.7 UI & Simulator Impact

- **Data Model**: Extend the blueprint data definitions (likely in `TechCatalog`) to include slots, preprints, extras, and faction modifiers so both `ShipLoadout` builders and the simulator consume identical metadata.
- **UI Validation**: The fleet builder must enforce the placement rules above (drive requirements, starbase bans, energy budget, tech ownership). Slot stacks should toggle between tile/preprint visuals per §11.2.
- **Simulator Inputs**: `ShipLoadout` instances generated from these blueprints must already respect energy and drive rules; simulator validation remains as a safety net.
- **Faction Customization**: Ensure UI defaults (auto-filled slots) use the faction-specific preprints rather than always Terran versions. Tests should confirm that Eridani energy bonuses, PLANTA initiative changes, and Orion shield overlays propagate into `ShipDerivedStats` and combat outcomes.

Documenting this reference here keeps the combat spec, ship editor, and tests synchronized with the tabletop ruleset. Future blueprint changes should update this section alongside `TechCatalog` data and UI validation logic.
