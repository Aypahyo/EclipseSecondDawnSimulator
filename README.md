# Eclipse Second Dawn Battle Simulator

An SDL2-powered fleet builder and probabilistic combat simulator for the board game *Eclipse: Second Dawn for the Galaxy*. Configure human and alien ship designs, drag technology tiles into valid slots, and run a cached dice-resolution engine that collapses redundant combat states (e.g., 2v2 encounters reused across simulations).

## Features

- **Visual fleet builder** – drag modules from the tech palette onto slot-compatible ship tiles, with automatic energy validation and right-click removal.
- **Design controls** – cycle through faction ship hulls using the `<` and `>` arrows on each card; every hull enforces engine and energy rules.
- **Deterministic battle math** – combats resolve with binomial dice distributions, simultaneous damage, and memoization of intermediate states to avoid dice explosions.
- **Status + summaries** – HUD callouts explain invalid configurations, while battle results report win/draw odds and expected rounds per fight.

## Building on Linux

Prerequisites:

- CMake ≥ 3.20
- A C++20 compiler (g++ 11+, clang++ 13+)
- SDL2 development headers (`libsdl2-dev` on Debian/Ubuntu)

```bash
sudo apt install build-essential cmake libsdl2-dev
cmake -S . -B build
cmake --build build
./build/eclipse_sim
```

The executable opens a 1280×720 window.

## Controls

| Action | Description |
| --- | --- |
| Drag from palette | Creates a module token of the hovered tech. Drop onto a slot with the same color/type. |
| Drag from slot | Moves an installed module; dropping outside any slot snaps it back. |
| Right-click slot | Removes the module from that slot. |
| `<` / `>` on card | Cycle through the available ship hulls for that faction. |
| **Simulate Battle** | Runs the cached probability simulation for the current fleets (requires all ships to be valid). |

Invalid drops (e.g., energy deficit or missing engine) trigger status messages under the palette until fixed.

## Architecture Notes

- `src/game/tech_catalog.cpp` – module stats and hull slot layouts for both factions.
- `src/game/battle_simulator.cpp` – recursive probability engine with memoized `BattleState` hashes.
- `src/render/bitmap_font.cpp` – tiny built-in 5×7 bitmap font so no extra font assets are required.
- `src/main.cpp` – SDL2 UI loop, drag-and-drop interactions, and integration between builder and simulator.

Feel free to extend the tech catalog, add more hulls, or plug in richer art/layouts. The simulation core already supports arbitrary hull stat mixtures, so new tiles or rules only require catalog tweaks.
