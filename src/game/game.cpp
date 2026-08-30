#include "game.hpp"
#include "../bots/snail.hpp"
#include "../math/intersection.hpp"
#include "map.hpp"

#include <format>
#include <iostream>

namespace game {

Racetrack::Racetrack(const std::string& mapFilePath, int numPlayers)
    : m_map(mapFilePath)
{
    const size_t startLength = m_map.start.points.size();

    for (int i = 0; i < numPlayers; ++i) {
        m_players.push_back(std::make_unique<bots::Snail>(static_cast<uint32_t>(i)));
        // Spread players evenly along start with equal distance between
        // players and the walls.
        const size_t p = i * startLength / (numPlayers + 2);
        m_playerStates.push_back({ .position = m_map.start.points[p],
            .velocity = { },
            .goal = 0,
            .active = true });
    }
}

void Racetrack::run()
{
    bool finished = false;
    size_t step = 0;

    while (!finished) {
        if (step % 10000 == 0) {
            std::cout << step << "\n";
        }

        bool noneActive = true;
        for (size_t player = 0; player < m_players.size(); ++player) {
            PlayerState& state = m_playerStates[player];
            if (state.active) {
                noneActive = false;
            } else {
                continue;
            }

            // player decision
            const Direction acceleration = m_players[player]->getAction(m_playerStates, m_map, player);
            if (acceleration.lenSq() > 2) {
                std::cout << std::format("[Warning] Ignoring Player {} input because of illegal action {}.\n", player, acceleration);
            } else {
                state.velocity += acceleration;
            }

            // movement step
            Point prevPos = state.position;
            state.position += state.velocity;
            const LineSegment seg = { prevPos, state.position };

            // check out of bounds
            if (m_map.intersectBoundary(seg)) {
                state.active = false;
                // skip goal check
                continue;
            }

            // check if goal was reached
            if (m_map.goals[state.goal].intersect(seg)) {
                ++state.goal;
                if (state.goal >= m_map.goals.size()) {
                    std::cout << std::format("Player {} finished!\n", player);
                    finished = true;
                } else {
                    std::cout << std::format("Player {} reached goal {}!\n", player, state.goal - 1);
                }
            }
        }

        // everyone crashed
        if (noneActive) {
            std::cout << "Game ends without a winner because everyone crashed.\n";
            finished = true;
        }

        ++step;
    }

	std::cout << std::format("Game ended after {} steps\n", step);
}

}
