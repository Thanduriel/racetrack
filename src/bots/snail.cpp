#include "snail.hpp"
#include "../game/map.hpp"

#include <numeric>

namespace bots {

using namespace game;

Snail::Snail(uint32_t seed)
    : m_prevGoal(-1)
    , m_currentDestination { }
    , m_rng(seed)
{
}

std::string Snail::name() { return "Snail"; }

Direction Snail::getAction(const GameState& gameState, const Map& map, const GameHistory& history)
{
    const PlayerState& state = gameState.getActive();

    // recompute destination
    if (m_prevGoal != state.goal) {
        Point posSum = { };
        const Line& goal = map.goals[state.goal];
        for (const Point& p : goal.points) {
            posSum += p;
        }
        m_currentDestination = posSum / static_cast<IType>(goal.points.size());
    }

    // collect valid actions
    std::vector<Direction> validActions;
    std::vector<float> distanceVals;
    for (const Direction& action : ACTIONS) {
        const Direction newVel = state.velocity + action;
        // too fast
        if (std::abs(newVel.x) > 1 || std::abs(newVel.y)  > 1) {
            continue;
        }

        // out of bounds
        const Point newPos = state.position + newVel;
        const LineSegment seg = { state.position, newPos };
        if (map.intersectBoundary(seg)) {
            continue;
        }

        validActions.push_back(action);
        distanceVals.push_back(math::distSq(newPos, m_currentDestination));
    }

    const float totalDist = std::accumulate(distanceVals.begin(), distanceVals.end(), 0.f);
    std::uniform_real_distribution<float> dist(0.f, totalDist);
    const float choice = dist(m_rng);
    float probSum = 0.f;
    size_t i = 0;
    for (float d : distanceVals) {
        probSum += d;
        if (choice < probSum) {
            return validActions[i];
        }
        ++i;
    }

    return ACTIONS[0];
}

}
