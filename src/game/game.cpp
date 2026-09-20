#include "game.hpp"
#include "../math/intersection.hpp"

#include <chrono>
#include <format>
#include <iostream>

namespace game {

constexpr bool stopOnFirst = false;

PlayerState& GameState::getCurrent()
{
	return playerStates[currentPlayer];
}
const PlayerState& GameState::getCurrent() const
{
	return playerStates[currentPlayer];
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

	//    auto lastTime = std::chrono::high_resolution_clock::now();

	while (!finished) {
		++step;
		/*   if (step % 10000 == 0) {
			   auto now = std::chrono::high_resolution_clock::now();
			   const float passed = std::chrono::duration<float>(now - lastTime).count();
			   std::cout << std::format("step {}, {}s\n", step, passed);
			   lastTime = now;
		   }*/

		bool noneActive = true;
		for (size_t player = 0; player < m_players.size(); ++player) {
			m_state.currentPlayer = player;
			PlayerState& state = m_state.getCurrent();
			if (state.active) {
				noneActive = false;
			} else {
				continue;
			}

			// player decision and movement
			const auto startTime = std::chrono::high_resolution_clock::now();
			Direction acceleration = m_players[m_state.currentPlayer]->getAction(m_state, m_map, m_history);
			const auto endTime = std::chrono::high_resolution_clock::now();
			const auto duration = std::chrono::duration<float>(endTime - startTime);
			std::cout << std::format("Player {} ({}) thought for {}s\n", m_state.currentPlayer, m_players[m_state.currentPlayer]->name(), duration.count());
			if (acceleration.lenSq() > 2) {
				std::cout << std::format("[Warning] Ignoring Player {} input because of illegal action {}.\n", m_state.currentPlayer, acceleration);
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
				std::cout << std::format("Player {} finished in {} steps!\n", player, step);
				state.active = false;
				if constexpr (stopOnFirst)
					finished = true;
			}
		}

		// everyone crashed or finished
		if (noneActive) {
			std::cout << "Game ends because there are no more active players.\n";
			finished = true;
		}
	}

	std::cout << std::format("Game ended after {} steps\n", step);
}

}
