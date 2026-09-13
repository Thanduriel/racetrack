#include "game.hpp"
#include "../math/intersection.hpp"

#include <chrono>
#include <format>
#include <iostream>

namespace game {

PlayerState& GameState::getActive()
{
    return playerStates[activePlayer];
}
const PlayerState& GameState::getActive() const
{
    return playerStates[activePlayer];
}

Racetrack::Racetrack(const Map& map, std::vector<std::unique_ptr<PlayerController>> players)
    : m_map(map)
    , m_players(std::move(players))
{
    const size_t startLength = m_map.start.points.size();
    const size_t numPlayers = m_players.size();

    for (size_t i = 0; i < numPlayers; ++i) {
        // Spread players evenly along start with equal distance between
        // players and the walls.
        const size_t p = i * startLength / (numPlayers + 2);
        m_state.playerStates.push_back({ .position = m_map.start.points[p],
            .velocity = { },
            .goal = 0,
            .active = true });
    }
}

void Racetrack::run()
{
    bool finished = false;
    size_t step = 0;

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!finished) {
        if (step % 10000 == 0) {
            auto now = std::chrono::high_resolution_clock::now();
            const float passed = std::chrono::duration<float>(now - lastTime).count();
            std::cout << std::format("step {}, {}s\n", step, passed);
            lastTime = now;
        }

        bool noneActive = true;
        for (size_t player = 0; player < m_players.size(); ++player) {
            PlayerState& state = m_state.getActive();
            if (state.active) {
                noneActive = false;
            } else {
                continue;
            }

            // player decision
            const Direction acceleration = m_players[m_state.activePlayer]->getAction(m_state, m_map);
            if (acceleration.lenSq() > 2) {
                std::cout << std::format("[Warning] Ignoring Player {} input because of illegal action {}.\n", m_state.activePlayer, acceleration);
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
                if (state.goal >= static_cast<int>(m_map.goals.size())) {
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
