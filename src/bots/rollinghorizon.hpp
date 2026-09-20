#pragma once

#include "../game/player.hpp"
#include <deque>
#include <vector>

namespace bots {
template <int MaxDepth>
class RollingHorizon : public game::PlayerController {
public:
	RollingHorizon()
	{
		size_t maxStates = 1;
		for (int i = 0; i < MaxDepth; ++i)
			maxStates *= game::ACTIONS.size();

		m_paths0.reserve(maxStates);
		m_paths1.reserve(maxStates);

		m_name = "RollingHorizon<" + std::to_string(MaxDepth) + ">";
	}

	std::string_view name() override { return m_name; }
	Direction getAction(const game::GameState& state, const game::Map& map, const game::GameHistory& history) override
	{
		// first exploration
		if (m_paths0.empty()) {
			// root
			m_paths0.push_back({ state.getCurrent(), 0.f, { } });

			for (int i = 0; i < MaxDepth; ++i) {
				m_paths1.clear();
				lookAhead(m_paths0, m_paths1, map);
				std::swap(m_paths0, m_paths1);
			}
		} else {
			Path dummyPath;
			dummyPath.actions[0] = m_previousAction;
			auto firstActionComp = [](const Path& lhs, const Path& rhs) {
				return lhs.actions[0] < rhs.actions[0];
			};

			const auto itStart = std::lower_bound(m_paths0.begin(), m_paths0.end(), dummyPath, firstActionComp);
			const auto itEnd = std::upper_bound(m_paths0.begin(), m_paths0.end(), dummyPath, firstActionComp);

			m_paths1.clear();
			lookAhead({ itStart, itEnd }, m_paths1, map);
			std::swap(m_paths0, m_paths1);
		}

		// every possible action leads to an invalid state
		if (m_paths0.empty()) {
			return { 0, 0 };
		}

		// determine best path
		const Path* bestPath = &m_paths0.front();
		for (const Path& path : m_paths0) {
			// heuristic for quality
			// 1. reached further goal
			// 2. faster
			if (path.state.goal > bestPath->state.goal
				|| path.distanceTraveled > bestPath->distanceTraveled) {
				bestPath = &path;
			}
		}

		m_previousAction = bestPath->actions[0];
		return game::ACTIONS[m_previousAction];
	}

private:
	struct Path {
		game::PlayerState state;
		float distanceTraveled; //< total distance since start
		std::array<uint8_t, MaxDepth> actions; //< actions taken to get here
	};

	void lookAhead(const std::span<Path>& current, std::vector<Path>& next, const game::Map& map) const
	{
		using namespace game;

		for (const Path& path : current) {
			// explore all possible moves
			uint8_t actionIdx = 0;
			for (Direction action : ACTIONS) {
				++actionIdx;
				PlayerState newState = path.state;
				const MOVE_RESULT result = advance(newState, action, map);

				if (result == MOVE_RESULT::INVALID) {
					continue;
				}

				// too fast to stop when seeing a wall
				if (newState.velocity.len() > static_cast<float>(MaxDepth)) {
					continue;
				}

				const float distTraveled = path.distanceTraveled + dist(newState.position, path.state.position);
				std::array<uint8_t, MaxDepth> newActions;
				std::copy(path.actions.begin() + 1, path.actions.end(), newActions.begin());
				newActions.back() = actionIdx - 1;

				next.push_back(Path { newState, distTraveled, newActions });
			}
		}
	}

	std::string m_name;

	std::vector<Path> m_paths0;
	std::vector<Path> m_paths1;

	uint8_t m_previousAction;
};
} // namespace bots
