#include <SDL.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "game/battle_simulator.hpp"
#include "game/tech_catalog.hpp"
#include "render/bitmap_font.hpp"

using namespace eclipse;

namespace {
struct PaletteEntry {
	const ModuleSpec* spec;
	SDL_Rect rect;
};

struct FleetShip {
	ShipLoadout loadout;
	size_t designIndex = 0;
	Faction faction = Faction::Human;
};

struct SlotRect {
	FleetShip* ship;
	size_t slotIndex;
	SDL_Rect rect;
};

struct ArrowRect {
	FleetShip* ship;
	SDL_Rect rect;
	int delta;
};

struct DragPayload {
	bool active = false;
	const ModuleSpec* spec = nullptr;
	FleetShip* sourceShip = nullptr;
	int sourceSlot = -1;
	bool fromPalette = false;
	int mouseX = 0;
	int mouseY = 0;
};

bool pointInRect(int x, int y, const SDL_Rect& rect) {
	return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

SDL_Color colorFromHex(uint32_t hex, uint8_t alpha = 255) {
	SDL_Color c;
	c.r = static_cast<uint8_t>((hex >> 16) & 0xFF);
	c.g = static_cast<uint8_t>((hex >> 8) & 0xFF);
	c.b = static_cast<uint8_t>(hex & 0xFF);
	c.a = alpha;
	return c;
}

const char* slotLabel(SlotType slot) {
	switch (slot) {
		case SlotType::Weapon: return "WEAPON";
		case SlotType::Drive: return "DRIVE";
		case SlotType::Computer: return "COMP";
		case SlotType::Shield: return "SHIELD";
		case SlotType::Power: return "POWER";
		case SlotType::Support: return "SUP";
	}
	return "";
}

SDL_Color slotColor(SlotType slot) {
	switch (slot) {
		case SlotType::Weapon: return colorFromHex(0xE07A5F);
		case SlotType::Drive: return colorFromHex(0x3D405B);
		case SlotType::Computer: return colorFromHex(0x81B29A);
		case SlotType::Shield: return colorFromHex(0xF2CC8F);
		case SlotType::Power: return colorFromHex(0x118AB2);
		case SlotType::Support: return colorFromHex(0x6C757D);
	}
	return colorFromHex(0xFFFFFF);
}

void drawPanel(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color fill, SDL_Color border) {
	SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
	SDL_RenderDrawRect(renderer, &rect);
}

SDL_Rect slotRectInCard(const SDL_Rect& cardRect, size_t slotIndex) {
	constexpr int columns = 3;
	constexpr int slotHeight = 60;
	int availableWidth = cardRect.w - 40;
	int spacing = 10;
	int slotWidth = (availableWidth - (columns - 1) * spacing);
	slotWidth = std::max(slotWidth / columns, 60);
	int row = static_cast<int>(slotIndex) / columns;
	int col = static_cast<int>(slotIndex) % columns;
	SDL_Rect rect;
	rect.x = cardRect.x + 20 + col * (slotWidth + spacing);
	rect.y = cardRect.y + 90 + row * (slotHeight + spacing);
	rect.w = slotWidth;
	rect.h = slotHeight;
	return rect;
}

std::pair<SDL_Rect, SDL_Rect> arrowRectsForCard(const SDL_Rect& rect) {
	SDL_Rect left{rect.x + 10, rect.y + 10, 30, 30};
	SDL_Rect right{rect.x + rect.w - 40, rect.y + 10, 30, 30};
	return {left, right};
}

void assignDesign(FleetShip& ship, const std::vector<const ShipDesign*>& options, size_t index,
				  const ModuleSpec* defaultDrive, const ModuleSpec* defaultWeapon) {
	if (options.empty()) {
		ship.loadout.setDesign(nullptr);
		return;
	}
	ship.designIndex = index % options.size();
	ship.loadout.setDesign(options[ship.designIndex]);
	auto& modules = ship.loadout.modules();
	bool drivePlaced = false;
	for (size_t i = 0; i < modules.size(); ++i) {
		if (ship.loadout.design()->slots[i] == SlotType::Drive && defaultDrive && !drivePlaced) {
			ship.loadout.setModule(i, defaultDrive);
			drivePlaced = true;
		}
	}
	bool weaponPlaced = false;
	for (size_t i = 0; i < modules.size(); ++i) {
		if (ship.loadout.design()->slots[i] == SlotType::Weapon && defaultWeapon && !weaponPlaced) {
			ship.loadout.setModule(i, defaultWeapon);
			weaponPlaced = true;
		}
	}
}

bool ensureModulePlacement(FleetShip& ship, size_t slotIndex, const ModuleSpec* spec) {
	if (!spec) {
		ship.loadout.clearModule(slotIndex);
		return true;
	}
	ship.loadout.setModule(slotIndex, spec);
	ShipDerivedStats stats = ship.loadout.derivedStats();
	if (stats.energyAvailable < stats.energyUsed) {
		ship.loadout.clearModule(slotIndex);
		return false;
	}
	return true;
}

std::vector<FleetShip> createFleet(Faction faction,
								   const std::vector<const ShipDesign*>& options,
								   const ModuleSpec* defaultDrive,
								   const ModuleSpec* defaultWeapon) {
	std::vector<FleetShip> fleet(3);
	for (size_t i = 0; i < fleet.size(); ++i) {
		fleet[i].faction = faction;
		if (!options.empty()) {
			assignDesign(fleet[i], options, std::min(i, options.size() - 1), defaultDrive, defaultWeapon);
		}
	}
	return fleet;
}

std::vector<PaletteEntry> buildPalette(const std::vector<const ModuleSpec*>& modules,
									   const SDL_Rect& area) {
	std::vector<PaletteEntry> entries;
	constexpr std::array<SlotType, 5> columnOrder = {
		SlotType::Weapon, SlotType::Drive, SlotType::Computer, SlotType::Shield, SlotType::Power};

	int columnWidth = area.w / static_cast<int>(columnOrder.size());
	int tileHeight = 70;
	int headerHeight = 28;

	for (size_t col = 0; col < columnOrder.size(); ++col) {
		SlotType slot = columnOrder[col];
		std::vector<const ModuleSpec*> filtered;
		std::copy_if(modules.begin(), modules.end(), std::back_inserter(filtered), [&](const ModuleSpec* spec) {
			return spec->slot == slot;
		});
		for (size_t i = 0; i < filtered.size(); ++i) {
			SDL_Rect rect;
			rect.x = area.x + static_cast<int>(col) * columnWidth + 10;
			rect.y = area.y + headerHeight + 10 + static_cast<int>(i) * (tileHeight + 10);
			rect.w = columnWidth - 20;
			rect.h = tileHeight;
			entries.push_back({filtered[i], rect});
		}
	}
	return entries;
}

std::vector<SDL_Rect> layoutCards(const SDL_Rect& area, size_t count) {
	std::vector<SDL_Rect> rects;
	if (count == 0) {
		return rects;
	}
	int spacing = 10;
	int cardWidth = (area.w - (static_cast<int>(count) + 1) * spacing) / static_cast<int>(count);
	int cardHeight = area.h - 2 * spacing;
	for (size_t i = 0; i < count; ++i) {
		SDL_Rect rect;
		rect.x = area.x + spacing + static_cast<int>(i) * (cardWidth + spacing);
		rect.y = area.y + spacing;
		rect.w = cardWidth;
		rect.h = cardHeight;
		rects.push_back(rect);
	}
	return rects;
}

std::vector<SlotRect> buildSlotRects(std::vector<FleetShip>& fleet,
									 const std::vector<SDL_Rect>& cardRects) {
	std::vector<SlotRect> slots;
	for (size_t i = 0; i < fleet.size() && i < cardRects.size(); ++i) {
		FleetShip& ship = fleet[i];
		for (size_t slotIndex = 0; slotIndex < ship.loadout.slotCount(); ++slotIndex) {
			slots.push_back({&ship, slotIndex, slotRectInCard(cardRects[i], slotIndex)});
		}
	}
	return slots;
}

std::vector<ArrowRect> buildArrows(std::vector<FleetShip>& fleet,
								   const std::vector<SDL_Rect>& cardRects) {
	std::vector<ArrowRect> arrows;
	for (size_t i = 0; i < fleet.size() && i < cardRects.size(); ++i) {
		auto [left, right] = arrowRectsForCard(cardRects[i]);
		arrows.push_back({&fleet[i], left, -1});
		arrows.push_back({&fleet[i], right, +1});
	}
	return arrows;
}

void renderPalette(SDL_Renderer* renderer, BitmapFont& font, const SDL_Rect& area,
				   const std::vector<PaletteEntry>& entries) {
	drawPanel(renderer, area, colorFromHex(0x1B1F3B), colorFromHex(0x394989));
	constexpr std::array<std::pair<SlotType, const char*>, 5> headers = {
		std::make_pair(SlotType::Weapon, "WEAPONS"),
		std::make_pair(SlotType::Drive, "DRIVES"),
		std::make_pair(SlotType::Computer, "COMPUTERS"),
		std::make_pair(SlotType::Shield, "SHIELDS"),
		std::make_pair(SlotType::Power, "POWER")};
	int columnWidth = area.w / static_cast<int>(headers.size());
	for (size_t i = 0; i < headers.size(); ++i) {
		int headerX = area.x + static_cast<int>(i) * columnWidth + 12;
		font.drawText(renderer, headers[i].second, headerX, area.y + 6, colorFromHex(0xF7FFF7), 2);
	}
	for (const PaletteEntry& entry : entries) {
		SDL_Color fill = slotColor(entry.spec->slot);
		drawPanel(renderer, entry.rect, fill, colorFromHex(0x000000));
		font.drawText(renderer, entry.spec->shortLabel, entry.rect.x + 6, entry.rect.y + 6,
					  colorFromHex(0x000000), 2);
		std::string energy = "E:" + std::to_string(entry.spec->energyCost) +
							 " P:" + std::to_string(entry.spec->energyProvided);
		font.drawText(renderer, energy, entry.rect.x + 6, entry.rect.y + 32, colorFromHex(0x000000), 2);
	}
	font.drawText(renderer, "Drag modules onto ships. Right click removes.",
				  area.x + 10, area.y + area.h - 100, colorFromHex(0xF4F1DE), 2);
}

void renderArrow(SDL_Renderer* renderer, const SDL_Rect& rect, bool isRight) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &rect);
	SDL_Point top = isRight ? SDL_Point{rect.x + 6, rect.y + 6} : SDL_Point{rect.x + rect.w - 6, rect.y + 6};
	SDL_Point mid = isRight ? SDL_Point{rect.x + rect.w - 6, rect.y + rect.h / 2}
							: SDL_Point{rect.x + 6, rect.y + rect.h / 2};
	SDL_Point bottom = isRight ? SDL_Point{rect.x + 6, rect.y + rect.h - 6}
							   : SDL_Point{rect.x + rect.w - 6, rect.y + rect.h - 6};
	SDL_RenderDrawLine(renderer, top.x, top.y, mid.x, mid.y);
	SDL_RenderDrawLine(renderer, mid.x, mid.y, bottom.x, bottom.y);
}

void renderShipCard(SDL_Renderer* renderer, BitmapFont& font, FleetShip& ship,
					const SDL_Rect& rect, bool isHuman) {
	SDL_Color fill = isHuman ? colorFromHex(0x14213D) : colorFromHex(0x2F1847);
	SDL_Color border = isHuman ? colorFromHex(0x3A506B) : colorFromHex(0x6247AA);
	drawPanel(renderer, rect, fill, border);

	auto [prevRect, nextRect] = arrowRectsForCard(rect);
	renderArrow(renderer, prevRect, false);
	renderArrow(renderer, nextRect, true);

	std::string title = ship.loadout.design() ? ship.loadout.design()->name : "NO DESIGN";
	font.drawText(renderer, title, rect.x + 50, rect.y + 16, colorFromHex(0xF0F4EF), 2);

	ShipDerivedStats stats = ship.loadout.derivedStats();
	std::ostringstream statLine;
	statLine << "H" << stats.hull << " D" << stats.dice << " C" << stats.computer
			 << " S" << stats.shield;
	font.drawText(renderer, statLine.str(), rect.x + 20, rect.y + 52, colorFromHex(0xF4F1BB), 2);

	std::string energyLine = "ENERGY " + std::to_string(stats.energyUsed) + "/" +
							 std::to_string(stats.energyAvailable);
	font.drawText(renderer, energyLine, rect.x + rect.w - 200, rect.y + 52, colorFromHex(0xF4F1BB), 2);

	std::string error = ship.loadout.validationError();
	if (!error.empty()) {
		font.drawText(renderer, error, rect.x + 20, rect.y + rect.h - 28, colorFromHex(0xEF233C), 2);
	}

	for (size_t slotIndex = 0; slotIndex < ship.loadout.slotCount(); ++slotIndex) {
		SDL_Rect slotRect = slotRectInCard(rect, slotIndex);
		SDL_Color fillColor = slotColor(ship.loadout.design()->slots[slotIndex]);
		drawPanel(renderer, slotRect, fillColor, colorFromHex(0x000000));
		font.drawText(renderer, slotLabel(ship.loadout.design()->slots[slotIndex]),
					  slotRect.x + 4, slotRect.y + 4, colorFromHex(0x000000), 2);
		if (const ModuleSpec* module = ship.loadout.moduleAt(slotIndex)) {
			font.drawText(renderer, module->shortLabel, slotRect.x + 4, slotRect.y + 32,
						  colorFromHex(0x000000), 2);
		}
	}
}

void renderDragGhost(SDL_Renderer* renderer, BitmapFont& font, const DragPayload& drag) {
	if (!drag.active || !drag.spec) {
		return;
	}
	SDL_Rect ghost{drag.mouseX - 50, drag.mouseY - 20, 100, 40};
	drawPanel(renderer, ghost, slotColor(drag.spec->slot), colorFromHex(0xFFFFFF));
	font.drawText(renderer, drag.spec->shortLabel, ghost.x + 6, ghost.y + 10, colorFromHex(0x000000), 2);
}

std::vector<ShipLoadout> collectFleet(const std::vector<FleetShip>& fleet) {
	std::vector<ShipLoadout> result;
	result.reserve(fleet.size());
	for (const FleetShip& ship : fleet) {
		result.push_back(ship.loadout);
	}
	return result;
}

bool fleetReady(const std::vector<FleetShip>& fleet) {
	if (fleet.empty()) {
		return false;
	}
	for (const FleetShip& ship : fleet) {
		if (!ship.loadout.isValid()) {
			return false;
		}
	}
	return true;
}

const std::vector<const ShipDesign*>& designOptionsFor(
	const FleetShip& ship,
	const std::vector<const ShipDesign*>& humanDesigns,
	const std::vector<const ShipDesign*>& alienDesigns) {
	return ship.faction == Faction::Human ? humanDesigns : alienDesigns;
}

}  // namespace

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Eclipse Second Dawn Battle Simulator",
										  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										  1280, 720, SDL_WINDOW_SHOWN);
	if (!window) {
		std::cerr << "Failed to create window: " << SDL_GetError() << "\n";
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::cerr << "Failed to create renderer: " << SDL_GetError() << "\n";
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	BitmapFont& font = BitmapFont::instance();

	SDL_Rect paletteRect{20, 20, 320, 680};
	SDL_Rect humanArea{360, 20, 900, 320};
	SDL_Rect alienArea{360, 360, 900, 320};
	SDL_Rect simulateButton{paletteRect.x + 20, paletteRect.y + paletteRect.h - 60,
							 paletteRect.w - 40, 40};

	const auto& moduleSpecs = TechCatalog::modules();
	std::vector<const ModuleSpec*> moduleRefs;
	for (const ModuleSpec& spec : moduleSpecs) {
		moduleRefs.push_back(&spec);
	}
	std::vector<PaletteEntry> paletteEntries = buildPalette(moduleRefs, paletteRect);

	std::vector<const ShipDesign*> humanDesigns = TechCatalog::factionDesigns(Faction::Human);
	std::vector<const ShipDesign*> alienDesigns = TechCatalog::factionDesigns(Faction::Alien);

	const ModuleSpec* defaultDrive = TechCatalog::findModule("NUCLEAR_DRIVE");
	const ModuleSpec* defaultWeapon = TechCatalog::findModule("ION_CANNON");

	std::vector<FleetShip> humanFleet = createFleet(Faction::Human, humanDesigns, defaultDrive, defaultWeapon);
	std::vector<FleetShip> alienFleet = createFleet(Faction::Alien, alienDesigns, defaultDrive, defaultWeapon);

	DragPayload drag;
	std::string statusMessage;
	Uint32 statusTimer = 0;
	auto setStatus = [&](const std::string& msg) {
		statusMessage = msg;
		statusTimer = SDL_GetTicks();
	};

	BattleSimulator simulator;
	BattleSummary summary;
	bool summaryReady = false;

	bool running = true;
	bool simulatePressed = false;

	while (running) {
		std::vector<SDL_Rect> humanCards = layoutCards(humanArea, humanFleet.size());
		std::vector<SDL_Rect> alienCards = layoutCards(alienArea, alienFleet.size());

		std::vector<SlotRect> slotRects = buildSlotRects(humanFleet, humanCards);
		std::vector<SlotRect> alienSlotRects = buildSlotRects(alienFleet, alienCards);
		slotRects.insert(slotRects.end(), alienSlotRects.begin(), alienSlotRects.end());

		std::vector<ArrowRect> arrows = buildArrows(humanFleet, humanCards);
		std::vector<ArrowRect> alienArrows = buildArrows(alienFleet, alienCards);
		arrows.insert(arrows.end(), alienArrows.begin(), alienArrows.end());

		auto findSlot = [&](int x, int y) -> SlotRect* {
			for (auto& slot : slotRects) {
				if (pointInRect(x, y, slot.rect)) {
					return &slot;
				}
			}
			return nullptr;
		};

		auto paletteAt = [&](int x, int y) -> const PaletteEntry* {
			for (const PaletteEntry& entry : paletteEntries) {
				if (pointInRect(x, y, entry.rect)) {
					return &entry;
				}
			}
			return nullptr;
		};

		auto cycleDesign = [&](FleetShip& ship, int delta) {
			const auto& options = designOptionsFor(ship, humanDesigns, alienDesigns);
			if (options.empty()) {
				return;
			}
			size_t newIndex = (ship.designIndex + options.size() + static_cast<size_t>(delta)) % options.size();
			assignDesign(ship, options, newIndex, defaultDrive, defaultWeapon);
			summaryReady = false;
			setStatus("Design updated.");
		};

		auto completeDrag = [&](int mx, int my) {
			if (!drag.active || !drag.spec) {
				drag.active = false;
				return;
			}
			bool placed = false;
			for (auto& slot : slotRects) {
				if (!slot.ship->loadout.isSlotCompatible(slot.slotIndex, *drag.spec)) {
					continue;
				}
				if (pointInRect(mx, my, slot.rect)) {
					if (ensureModulePlacement(*slot.ship, slot.slotIndex, drag.spec)) {
						placed = true;
						summaryReady = false;
						setStatus("Module installed.");
					} else {
						setStatus("Not enough energy for module.");
					}
					break;
				}
			}
			if (!placed && !drag.fromPalette && drag.sourceShip && drag.sourceSlot >= 0) {
				ensureModulePlacement(*drag.sourceShip, drag.sourceSlot, drag.spec);
			}
			drag.active = false;
		};

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						running = false;
					}
					break;
				case SDL_MOUSEBUTTONDOWN: {
					int mx = event.button.x;
					int my = event.button.y;
					if (event.button.button == SDL_BUTTON_LEFT) {
						bool handled = false;
						for (auto& arrow : arrows) {
							if (pointInRect(mx, my, arrow.rect)) {
								cycleDesign(*arrow.ship, arrow.delta);
								handled = true;
								break;
							}
						}
						if (!handled) {
							if (const PaletteEntry* entry = paletteAt(mx, my)) {
								drag.active = true;
								drag.spec = entry->spec;
								drag.sourceShip = nullptr;
								drag.sourceSlot = -1;
								drag.fromPalette = true;
								drag.mouseX = mx;
								drag.mouseY = my;
								handled = true;
							}
						}
						if (!handled) {
							if (SlotRect* slot = findSlot(mx, my)) {
								if (const ModuleSpec* module = slot->ship->loadout.moduleAt(slot->slotIndex)) {
									drag.active = true;
									drag.spec = module;
									drag.sourceShip = slot->ship;
									drag.sourceSlot = static_cast<int>(slot->slotIndex);
									drag.fromPalette = false;
									drag.mouseX = mx;
									drag.mouseY = my;
									slot->ship->loadout.clearModule(slot->slotIndex);
									handled = true;
								}
							}
						}
						if (!handled && pointInRect(mx, my, simulateButton)) {
							simulatePressed = true;
						}
					} else if (event.button.button == SDL_BUTTON_RIGHT) {
						if (SlotRect* slot = findSlot(mx, my)) {
							slot->ship->loadout.clearModule(slot->slotIndex);
							summaryReady = false;
							setStatus("Module removed.");
						}
					}
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					int mx = event.button.x;
					int my = event.button.y;
					if (event.button.button == SDL_BUTTON_LEFT) {
						if (drag.active) {
							completeDrag(mx, my);
						} else if (simulatePressed && pointInRect(mx, my, simulateButton)) {
							if (!fleetReady(humanFleet) || !fleetReady(alienFleet)) {
								setStatus("All ships need engines and enough energy before simulating.");
							} else {
								summary = simulator.simulate(collectFleet(humanFleet), collectFleet(alienFleet));
								summaryReady = true;
								setStatus("Simulation complete.");
							}
						}
						simulatePressed = false;
					}
					break;
				}
				case SDL_MOUSEMOTION:
					if (drag.active) {
						drag.mouseX = event.motion.x;
						drag.mouseY = event.motion.y;
					}
					break;
				default:
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 14, 20, 37, 255);
		SDL_RenderClear(renderer);

		renderPalette(renderer, font, paletteRect, paletteEntries);

		for (size_t i = 0; i < humanFleet.size() && i < humanCards.size(); ++i) {
			renderShipCard(renderer, font, humanFleet[i], humanCards[i], true);
		}
		for (size_t i = 0; i < alienFleet.size() && i < alienCards.size(); ++i) {
			renderShipCard(renderer, font, alienFleet[i], alienCards[i], false);
		}

		SDL_Color buttonColor = simulatePressed ? colorFromHex(0xFF9F1C) : colorFromHex(0x2EC4B6);
		drawPanel(renderer, simulateButton, buttonColor, colorFromHex(0x011627));
		font.drawText(renderer, "SIMULATE BATTLE", simulateButton.x + 20, simulateButton.y + 10,
					  colorFromHex(0x011627), 2);

		if (summaryReady) {
			std::ostringstream lines;
			lines.precision(1);
			lines << std::fixed;
			lines << "HUMAN WIN " << summary.humanWin * 100.0 << "%\n";
			lines << "ALIEN WIN " << summary.alienWin * 100.0 << "%\n";
			lines << "DRAW " << summary.draw * 100.0 << "%\n";
			lines << "EXP ROUNDS " << summary.expectedRounds;
			font.drawText(renderer, lines.str(), paletteRect.x + 10, paletteRect.y + paletteRect.h - 180,
						  colorFromHex(0xF1FAEE), 2);
		}

		if (!statusMessage.empty() && SDL_GetTicks() - statusTimer < 4000) {
			font.drawText(renderer, statusMessage, paletteRect.x + 10, paletteRect.y + paletteRect.h - 24,
						  colorFromHex(0xFF5964), 2);
		}

		renderDragGhost(renderer, font, drag);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
