#include "game/battle_simulator.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>

namespace eclipse {

namespace {
struct BattleShipProfile {
    int hull;
    int dice;
    int computer;
    int shield;

    bool operator==(const BattleShipProfile& other) const {
        return hull == other.hull &&
               dice == other.dice &&
               computer == other.computer &&
               shield == other.shield;
    }
};

struct BattleState {
    std::vector<BattleShipProfile> humans;
    std::vector<BattleShipProfile> aliens;

    bool operator==(const BattleState& other) const {
        return humans == other.humans && aliens == other.aliens;
    }
};

struct StateHash {
    std::size_t operator()(const BattleState& state) const noexcept {
        auto hashVec = [](const std::vector<BattleShipProfile>& vec) {
            std::size_t h = 0;
            for (const auto& ship : vec) {
                std::size_t segment = static_cast<std::size_t>(ship.hull & 0xFF);
                segment = (segment << 8) ^ static_cast<std::size_t>(ship.dice & 0xFF);
                segment = (segment << 8) ^ static_cast<std::size_t>((ship.computer & 0xFF));
                segment = (segment << 8) ^ static_cast<std::size_t>((ship.shield & 0xFF));
                h ^= segment + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            }
            return h;
        };
        std::size_t hash = hashVec(state.humans);
        hash ^= hashVec(state.aliens) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        return hash;
    }
};

bool battleCompare(const BattleShipProfile& a, const BattleShipProfile& b) {
    if (a.hull != b.hull) return a.hull > b.hull;
    if (a.dice != b.dice) return a.dice > b.dice;
    if (a.computer != b.computer) return a.computer > b.computer;
    return a.shield > b.shield;
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
                                    const std::vector<BattleShipProfile>& defenders) {
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
        if (ship.dice <= 0) {
            continue;
        }
        int delta = ship.computer - static_cast<int>(std::round(avgShield));
        int threshold = 6 - delta;
        if (threshold < 2) threshold = 2;
        if (threshold > 6) threshold = 6;
        double success = clamp01((7 - threshold) / 6.0);
        std::vector<double> shipDist = binomialDistribution(ship.dice, success);
        distribution = convolve(distribution, shipDist);
    }
    return distribution;
}

std::vector<BattleShipProfile> applyHits(const std::vector<BattleShipProfile>& defenders, int hits) {
    if (hits <= 0 || defenders.empty()) {
        return defenders;
    }
    std::vector<BattleShipProfile> remaining = defenders;
    std::sort(remaining.begin(), remaining.end(), [](const BattleShipProfile& a, const BattleShipProfile& b) {
        if (a.hull != b.hull) return a.hull < b.hull;
        if (a.dice != b.dice) return a.dice < b.dice;
        if (a.computer != b.computer) return a.computer < b.computer;
        return a.shield < b.shield;
    });
    size_t index = 0;
    int damage = hits;
    while (damage > 0 && index < remaining.size()) {
        BattleShipProfile& ship = remaining[index];
        int applied = std::min(damage, ship.hull);
        ship.hull -= applied;
        damage -= applied;
        if (ship.hull <= 0) {
            remaining.erase(remaining.begin() + static_cast<long>(index));
        } else {
            ++index;
        }
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

    auto humanHits = hitDistribution(state.humans, state.aliens);
    auto alienHits = hitDistribution(state.aliens, state.humans);

    double stayProbability = 0.0;
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
            BattleState next;
            next.humans = applyHits(state.humans, static_cast<int>(a));
            next.aliens = applyHits(state.aliens, static_cast<int>(h));
            canonicalize(next);
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
    auto toProfiles = [](const std::vector<ShipLoadout>& fleet) {
        std::vector<BattleShipProfile> profiles;
        profiles.reserve(fleet.size());
        for (const ShipLoadout& ship : fleet) {
            if (!ship.isValid()) {
                continue;
            }
            ShipDerivedStats stats = ship.derivedStats();
            BattleShipProfile profile;
            profile.hull = std::max(1, stats.hull);
            profile.dice = std::max(0, stats.dice);
            profile.computer = stats.computer;
            profile.shield = stats.shield;
            profiles.push_back(profile);
        }
        std::sort(profiles.begin(), profiles.end(), battleCompare);
        return profiles;
    };

    BattleState state;
    state.humans = toProfiles(humans);
    state.aliens = toProfiles(aliens);
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
