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
    , m_history(m_players.size())
{
    const auto& startPoints = m_map.start.points;
    const size_t startLength = startPoints.size();
    const size_t numPlayers = m_players.size();

    if (startLength < numPlayers) {
        std::cerr << "[Error] There are more players than starting positions.\n";
        std::abort();
    }

    for (size_t i = 0; i < numPlayers; ++i) {
        // Spread players evenly along start with equal distance between
        // players and the walls.
        const size_t p = i * startLength / (numPlayers + 2);
        m_state.playerStates.push_back({ .position = startPoints[p],
            .velocity = { },
            .goal = 0,
            .active = true });

        m_history[i].points.push_back(startPoints[p]);
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
            m_state.activePlayer = player;
            PlayerState& state = m_state.getActive();
            if (state.active) {
                noneActive = false;
            } else {
                continue;
            }

            // player decision and movement
            Direction acceleration = m_players[m_state.activePlayer]->getAction(m_state, m_map, m_history);
            if (acceleration.lenSq() > 2) {
                std::cout << std::format("[Warning] Ignoring Player {} input because of illegal action {}.\n", m_state.activePlayer, acceleration);
                acceleration = { 0, 0 };
            }
            const MOVE_RESULT result = advance(state, acceleration, m_map);

            // record history
            m_history[player].points.push_back(state.position);

            // check outcome
            if (result == MOVE_RESULT::INVALID) {
                std::cout << std::format("Player {} crashed!\n", player);
            } else if (result == MOVE_RESULT::PASSED_GOAL) {
                std::cout << std::format("Player {} reached goal {}!\n", player, state.goal - 1);
            } else if (result == MOVE_RESULT::PASSED_FINAL_GOAL) {
                std::cout << std::format("Player {} finished!\n", player);
                finished = true;
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
