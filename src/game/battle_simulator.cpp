#include "game/battle_simulator.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numeric>
#include <optional>
#include <unordered_map>

namespace eclipse {

namespace {
struct WeaponStats {
    int dice = 0;
    int dieSides = 6;
    int baseToHit = 6;
    int initiative = 1;
    bool missile = false;
    bool oneShot = false;

    bool operator==(const WeaponStats& other) const {
        return dice == other.dice && dieSides == other.dieSides && baseToHit == other.baseToHit &&
               initiative == other.initiative && missile == other.missile && oneShot == other.oneShot;
    }
};

struct BattleShipProfile {
    int hull = 0;
    int computer = 0;
    int shield = 0;
    bool fluxShield = false;
    ShipClass shipClass = ShipClass::Other;
    std::vector<WeaponStats> weapons;
    std::vector<WeaponStats> missiles;

    bool operator==(const BattleShipProfile& other) const {
        return hull == other.hull && computer == other.computer && shield == other.shield &&
               fluxShield == other.fluxShield && shipClass == other.shipClass &&
               weapons == other.weapons && missiles == other.missiles;
    }
};

struct BattleState {
    std::vector<BattleShipProfile> humans;
    std::vector<BattleShipProfile> aliens;
    bool missilesResolved = false;

    bool operator==(const BattleState& other) const {
        return humans == other.humans && aliens == other.aliens && missilesResolved == other.missilesResolved;
    }
};

struct StateHash {
    std::size_t operator()(const BattleState& state) const noexcept {
        auto hashWeapons = [](const std::vector<WeaponStats>& vec) {
            std::size_t h = 0;
            for (const auto& ship : vec) {
                std::size_t segment = static_cast<std::size_t>(ship.dice & 0xFF);
                segment = (segment << 8) ^ static_cast<std::size_t>(ship.dieSides & 0xFF);
                segment = (segment << 8) ^ static_cast<std::size_t>(ship.baseToHit & 0xFF);
                segment = (segment << 8) ^ static_cast<std::size_t>(ship.initiative & 0xFF);
                segment ^= static_cast<std::size_t>(ship.missile ? 0x4000 : 0x0);
                segment ^= static_cast<std::size_t>(ship.oneShot ? 0x8000 : 0x0);
                h ^= segment + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            }
            return h;
        };
        auto hashVec = [&](const std::vector<BattleShipProfile>& vec) {
            std::size_t h = 0;
            for (const auto& ship : vec) {
                std::size_t segment = static_cast<std::size_t>(ship.hull & 0xFF);
                segment = (segment << 8) ^ static_cast<std::size_t>((ship.computer & 0xFF));
                segment = (segment << 8) ^ static_cast<std::size_t>((ship.shield & 0xFF));
                segment ^= static_cast<std::size_t>(ship.fluxShield ? 0x1000 : 0x0);
                segment ^= static_cast<std::size_t>(static_cast<int>(ship.shipClass) & 0xFF) << 24;
                std::size_t weaponHash = hashWeapons(ship.weapons);
                std::size_t missileHash = hashWeapons(ship.missiles);
                segment ^= weaponHash;
                segment ^= missileHash << 1;
                h ^= segment + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            }
            return h;
        };
        std::size_t hash = hashVec(state.humans);
        hash ^= hashVec(state.aliens) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        hash ^= static_cast<std::size_t>(state.missilesResolved ? 0xFFFF : 0x1);
        return hash;
    }
};

int totalDice(const std::vector<WeaponStats>& weapons) {
    int total = 0;
    for (const auto& weapon : weapons) {
        total += weapon.dice;
    }
    return total;
}

int totalDice(const BattleShipProfile& ship) {
    return totalDice(ship.weapons);
}

bool battleCompare(const BattleShipProfile& a, const BattleShipProfile& b) {
    if (a.hull != b.hull) return a.hull > b.hull;
    int diceA = totalDice(a);
    int diceB = totalDice(b);
    if (diceA != diceB) return diceA > diceB;
    if (a.computer != b.computer) return a.computer > b.computer;
    if (a.shield != b.shield) return a.shield > b.shield;
    if (a.fluxShield != b.fluxShield) return a.fluxShield;
    return static_cast<int>(a.shipClass) < static_cast<int>(b.shipClass);
}

void canonicalize(BattleState& state) {
    std::sort(state.humans.begin(), state.humans.end(), battleCompare);
    std::sort(state.aliens.begin(), state.aliens.end(), battleCompare);
}

double clamp01(double value) {
    if (value < 0.0) return 0.0;
    if (value > 1.0) return 1.0;
    return value;
}

std::vector<double> binomialDistribution(int dice, double success) {
    if (dice <= 0) {
        return {1.0};
    }
    std::vector<double> dist(dice + 1, 0.0);
    dist[0] = 1.0;
    for (int d = 0; d < dice; ++d) {
        for (int hits = d + 1; hits >= 0; --hits) {
            double stay = dist[hits] * (1.0 - success);
            double advance = (hits > 0 ? dist[hits - 1] : 0.0) * success;
            dist[hits] = stay + advance;
        }
    }
    return dist;
}

std::vector<double> convolve(const std::vector<double>& lhs, const std::vector<double>& rhs) {
    std::vector<double> result(lhs.size() + rhs.size() - 1, 0.0);
    for (size_t i = 0; i < lhs.size(); ++i) {
        for (size_t j = 0; j < rhs.size(); ++j) {
            result[i + j] += lhs[i] * rhs[j];
        }
    }
    return result;
}

std::vector<double> hitDistribution(const std::vector<BattleShipProfile>& attackers,
                                    const std::vector<BattleShipProfile>& defenders,
                                    bool missilesOnly,
                                    std::optional<int> initiativeFilter = std::nullopt) {
    if (attackers.empty() || defenders.empty()) {
        return {1.0};
    }
    std::vector<double> distribution{1.0};
    double shieldSum = 0.0;
    for (const auto& ship : defenders) {
        shieldSum += ship.shield;
    }
    double avgShield = defenders.empty() ? 0.0 : (shieldSum / defenders.size());

    for (const auto& ship : attackers) {
        const auto& pools = missilesOnly ? ship.missiles : ship.weapons;
        if (pools.empty()) {
            continue;
        }
        int delta = ship.computer - static_cast<int>(std::round(avgShield));
        for (const auto& weapon : pools) {
            if (weapon.dice <= 0) {
                continue;
            }
            if (missilesOnly && !weapon.missile) {
                continue;
            }
            if (!missilesOnly && weapon.missile) {
                continue;
            }
            if (initiativeFilter.has_value() && weapon.initiative != *initiativeFilter) {
                continue;
            }
            int threshold = weapon.baseToHit - delta;
            int maxRoll = std::max(weapon.dieSides, 2);
            if (threshold < 2) threshold = 2;
            if (threshold > maxRoll) threshold = maxRoll;
            double success = clamp01(((maxRoll + 1) - threshold) / static_cast<double>(maxRoll));
            std::vector<double> shipDist = binomialDistribution(weapon.dice, success);
            distribution = convolve(distribution, shipDist);
        }
    }
    return distribution;
}

std::vector<BattleShipProfile> applyHits(const std::vector<BattleShipProfile>& defenders, int hits) {
    if (hits <= 0 || defenders.empty()) {
        return defenders;
    }
    struct TargetShip {
        BattleShipProfile ship;
        bool fluxConsumed = false;
    };
    std::vector<TargetShip> targets;
    targets.reserve(defenders.size());
    for (const auto& ship : defenders) {
        targets.push_back(TargetShip{ship, false});
    }
    std::sort(targets.begin(), targets.end(), [](const TargetShip& a, const TargetShip& b) {
        if (a.ship.hull != b.ship.hull) return a.ship.hull < b.ship.hull;
        int diceA = totalDice(a.ship);
        int diceB = totalDice(b.ship);
        if (diceA != diceB) return diceA < diceB;
        if (a.ship.computer != b.ship.computer) return a.ship.computer < b.ship.computer;
        return a.ship.shield < b.ship.shield;
    });
    size_t index = 0;
    int damage = hits;
    while (damage > 0 && index < targets.size()) {
        TargetShip& target = targets[index];
        int rawDamage = std::min(damage, target.ship.hull);
        int prevention = 0;
        if (target.ship.fluxShield && !target.fluxConsumed && rawDamage > 0) {
            prevention = 1;
            target.fluxConsumed = true;
        }
        int effectiveDamage = std::max(0, rawDamage - prevention);
        if (effectiveDamage == 0) {
            ++index;
            continue;
        }
        target.ship.hull -= effectiveDamage;
        damage -= effectiveDamage;
        if (target.ship.hull <= 0) {
            targets.erase(targets.begin() + static_cast<long>(index));
        } else {
            ++index;
        }
    }
    std::vector<BattleShipProfile> remaining;
    remaining.reserve(targets.size());
    for (auto& target : targets) {
        remaining.push_back(std::move(target.ship));
    }
    std::sort(remaining.begin(), remaining.end(), battleCompare);
    return remaining;
}

struct CachedResult {
    double humanWin;
    double alienWin;
    double draw;
    double expectedRounds;
};

CachedResult solveState(const BattleState& state,
                        std::unordered_map<BattleState, CachedResult, StateHash>& cache);

bool fleetHasMissiles(const std::vector<BattleShipProfile>& fleet) {
    for (const auto& ship : fleet) {
        if (!ship.missiles.empty()) {
            return true;
        }
    }
    return false;
}

void clearMissiles(std::vector<BattleShipProfile>& fleet) {
    for (auto& ship : fleet) {
        ship.missiles.clear();
    }
}

std::vector<int> collectInitiatives(const std::vector<BattleShipProfile>& humans,
                                    const std::vector<BattleShipProfile>& aliens) {
    std::vector<int> initiatives;
    auto addFromFleet = [&](const std::vector<BattleShipProfile>& fleet) {
        for (const auto& ship : fleet) {
            for (const auto& weapon : ship.weapons) {
                if (std::find(initiatives.begin(), initiatives.end(), weapon.initiative) == initiatives.end()) {
                    initiatives.push_back(weapon.initiative);
                }
            }
        }
    };
    addFromFleet(humans);
    addFromFleet(aliens);
    std::sort(initiatives.begin(), initiatives.end(), std::greater<>());
    return initiatives;
}

void accumulateInitiativeOutcomes(const BattleState& current,
                                  const std::vector<int>& initiatives,
                                  size_t index,
                                  double probability,
                                  std::unordered_map<BattleState, double, StateHash>& accumulator) {
    if (initiatives.empty() || index >= initiatives.size()) {
        BattleState terminal = current;
        canonicalize(terminal);
        accumulator[terminal] += probability;
        return;
    }

    int initiative = initiatives[index];
    auto humanHits = hitDistribution(current.humans, current.aliens, false, initiative);
    auto alienHits = hitDistribution(current.aliens, current.humans, false, initiative);

    for (size_t h = 0; h < humanHits.size(); ++h) {
        for (size_t a = 0; a < alienHits.size(); ++a) {
            double pairProb = humanHits[h] * alienHits[a];
            if (pairProb <= 0.0) {
                continue;
            }
            BattleState next = current;
            next.humans = applyHits(current.humans, static_cast<int>(a));
            next.aliens = applyHits(current.aliens, static_cast<int>(h));
            accumulateInitiativeOutcomes(next, initiatives, index + 1, probability * pairProb, accumulator);
        }
    }
}

CachedResult resolveMissilePhase(const BattleState& state,
                                 std::unordered_map<BattleState, CachedResult, StateHash>& cache) {
    bool humanMissiles = fleetHasMissiles(state.humans);
    bool alienMissiles = fleetHasMissiles(state.aliens);
    if (!humanMissiles && !alienMissiles) {
        BattleState next = state;
        next.missilesResolved = true;
        return solveState(next, cache);
    }

    auto humanHits = hitDistribution(state.humans, state.aliens, true, std::nullopt);
    auto alienHits = hitDistribution(state.aliens, state.humans, true, std::nullopt);

    double progressProbability = 0.0;
    double humanAccum = 0.0;
    double alienAccum = 0.0;
    double drawAccum = 0.0;
    double childRounds = 0.0;

    for (size_t h = 0; h < humanHits.size(); ++h) {
        for (size_t a = 0; a < alienHits.size(); ++a) {
            double pairProb = humanHits[h] * alienHits[a];
            if (pairProb <= 0.0) {
                continue;
            }
            BattleState next = state;
            next.humans = applyHits(state.humans, static_cast<int>(a));
            next.aliens = applyHits(state.aliens, static_cast<int>(h));
            next.missilesResolved = true;
            clearMissiles(next.humans);
            clearMissiles(next.aliens);
            canonicalize(next);
            CachedResult child = solveState(next, cache);
            progressProbability += pairProb;
            humanAccum += pairProb * child.humanWin;
            alienAccum += pairProb * child.alienWin;
            drawAccum += pairProb * child.draw;
            childRounds += pairProb * child.expectedRounds;
        }
    }

    CachedResult result;
    if (progressProbability <= std::numeric_limits<double>::epsilon()) {
        result = {0.0, 0.0, 1.0, 0.0};
    } else {
        result.humanWin = humanAccum / progressProbability;
        result.alienWin = alienAccum / progressProbability;
        result.draw = drawAccum / progressProbability;
        result.expectedRounds = childRounds / progressProbability;
    }
    return result;
}

CachedResult solveState(const BattleState& state,
                        std::unordered_map<BattleState, CachedResult, StateHash>& cache) {
    if (state.humans.empty() && state.aliens.empty()) {
        return {0.0, 0.0, 1.0, 0.0};
    }
    if (state.aliens.empty()) {
        return {1.0, 0.0, 0.0, 0.0};
    }
    if (state.humans.empty()) {
        return {0.0, 1.0, 0.0, 0.0};
    }

    auto it = cache.find(state);
    if (it != cache.end()) {
        return it->second;
    }

    if (!state.missilesResolved) {
        CachedResult missileResult = resolveMissilePhase(state, cache);
        cache.emplace(state, missileResult);
        return missileResult;
    }

    double stayProbability = 0.0;
    double progressProbability = 0.0;
    double humanAccum = 0.0;
    double alienAccum = 0.0;
    double drawAccum = 0.0;
    double childRounds = 0.0;

    std::vector<int> initiatives = collectInitiatives(state.humans, state.aliens);
    std::unordered_map<BattleState, double, StateHash> nextStates;
    if (initiatives.empty()) {
        BattleState terminal = state;
        canonicalize(terminal);
        nextStates[terminal] = 1.0;
    } else {
        accumulateInitiativeOutcomes(state, initiatives, 0, 1.0, nextStates);
    }

    for (const auto& entry : nextStates) {
        const BattleState& next = entry.first;
        double pairProb = entry.second;
        if (pairProb <= 0.0) {
            continue;
        }
        if (next == state) {
            stayProbability += pairProb;
            continue;
        }
        CachedResult child = solveState(next, cache);
        progressProbability += pairProb;
        humanAccum += pairProb * child.humanWin;
        alienAccum += pairProb * child.alienWin;
        drawAccum += pairProb * child.draw;
        childRounds += pairProb * child.expectedRounds;
    }

    CachedResult result;
    if (progressProbability <= std::numeric_limits<double>::epsilon()) {
        // Stalemate configuration, treat as a draw.
        result = {0.0, 0.0, 1.0, 0.0};
    } else {
        result.humanWin = humanAccum / progressProbability;
        result.alienWin = alienAccum / progressProbability;
        result.draw = drawAccum / progressProbability;
        result.expectedRounds = (1.0 + childRounds) / progressProbability;
    }

    cache.emplace(state, result);
    return result;
}

BattleState buildState(const std::vector<ShipLoadout>& humans,
                       const std::vector<ShipLoadout>& aliens) {
    auto limitForClass = [](ShipClass cls) {
        switch (cls) {
            case ShipClass::Interceptor:
                return 8;
            case ShipClass::Cruiser:
                return 4;
            case ShipClass::Dreadnought:
                return 2;
            case ShipClass::Starbase:
                return 1;
            default:
                return std::numeric_limits<int>::max();
        }
    };

    auto makeProfile = [](const ShipLoadout& ship) {
        BattleShipProfile profile;
        const ShipDesign* design = ship.design();
        ShipDerivedStats stats = ship.derivedStats();
        profile.hull = std::max(1, stats.hull);
        profile.computer = stats.computer;
        profile.shield = stats.shield;
        profile.shipClass = design ? design->shipClass : ShipClass::Other;
        int initiativeBonus = stats.initiativeBonus;

        if (design && design->baseDice > 0) {
            WeaponStats baseWeapon;
            baseWeapon.dice = design->baseDice;
            baseWeapon.dieSides = design->baseWeaponDieSides;
            baseWeapon.baseToHit = design->baseWeaponHit;
            baseWeapon.initiative = design->baseWeaponInitiative + initiativeBonus;
            profile.weapons.push_back(baseWeapon);
        }

        std::vector<const ModuleSpec*> activeModules = ship.activeModules();
        for (const ModuleSpec* module : activeModules) {
            if (module->grantsFluxShield) {
                profile.fluxShield = true;
            }
            if (module->dice > 0) {
                WeaponStats weapon;
                weapon.dice = module->dice;
                weapon.dieSides = module->weaponDieSides;
                weapon.baseToHit = module->baseToHit;
                weapon.initiative = module->missile ? module->weaponInitiative
                                                    : module->weaponInitiative + initiativeBonus;
                weapon.missile = module->missile;
                weapon.oneShot = module->oneShot;
                if (weapon.missile) {
                    profile.missiles.push_back(weapon);
                } else {
                    profile.weapons.push_back(weapon);
                }
            }
        }
        return profile;
    };

    auto toProfiles = [&](const std::vector<ShipLoadout>& fleet) {
        std::vector<BattleShipProfile> profiles;
        profiles.reserve(fleet.size());
        std::array<int, 5> counts{};
        for (const ShipLoadout& ship : fleet) {
            if (!ship.isValid()) {
                continue;
            }
            const ShipDesign* design = ship.design();
            ShipClass cls = design ? design->shipClass : ShipClass::Other;
            int limit = limitForClass(cls);
            size_t idx = static_cast<size_t>(cls);
            if (idx >= counts.size()) {
                idx = counts.size() - 1;
            }
            if (counts[idx] >= limit) {
                continue;
            }
            counts[idx] += 1;
            profiles.push_back(makeProfile(ship));
        }
        std::sort(profiles.begin(), profiles.end(), battleCompare);
        return profiles;
    };

    BattleState state;
    state.humans = toProfiles(humans);
    state.aliens = toProfiles(aliens);
    state.missilesResolved = false;
    return state;
}

}  // namespace

BattleSummary BattleSimulator::simulate(const std::vector<ShipLoadout>& humans,
                                        const std::vector<ShipLoadout>& aliens) {
    BattleState state = buildState(humans, aliens);
    canonicalize(state);
    std::unordered_map<BattleState, CachedResult, StateHash> cache;
    CachedResult result = solveState(state, cache);
    BattleSummary summary;
    summary.humanWin = result.humanWin;
    summary.alienWin = result.alienWin;
    summary.draw = result.draw;
    summary.expectedRounds = result.expectedRounds;
    return summary;
}

}  // namespace eclipse
