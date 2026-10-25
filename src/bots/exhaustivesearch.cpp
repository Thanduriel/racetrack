#include "exhaustivesearch.hpp"

namespace bots {

using namespace game;

StateCache::StateCache(const PlayerState& baseState)
{
	m_cache[baseState] = { 0, baseState };
}

std::optional<int> StateCache::add(const PlayerState& state, const PlayerState& prevState)
{
	auto prevIt = m_cache.find(prevState);
	if (prevIt == m_cache.end()) {
		std::cerr << "[Error] Inconsistent cache state\n";
		std::abort();
	}
	const int steps = prevIt->second.steps + 1;

	const auto& [newIt, inserted] = m_cache.try_emplace(state, steps, prevState);
	auto& info = newIt->second;
	if (!inserted) {
		if (info.steps <= steps) {
			return std::nullopt;
		} else {
			info.steps = steps;
			info.previous = prevState;
		}
	}
	return steps;
}

const StateCache::PathInfo& StateCache::at(const game::PlayerState& state) const
{
	return m_cache.at(state);
}

int StateCache::longestPath() const
{
	return std::max_element(m_cache.begin(), m_cache.end(), [](const auto& lhs, const auto& rhs) {
		return lhs.second.steps < rhs.second.steps;
	})->second.steps;
}

// ********************************************** //
ExhaustiveSearch::ExhaustiveSearch(std::unique_ptr<game::PlayerController> preconditioner)
	: m_preconditioner(std::move(preconditioner))
{
}

std::string_view ExhaustiveSearch::name() { return "ExhaustiveSearch"; }

Direction ExhaustiveSearch::getAction(const GameState& state, const Map& map, const GameHistory& history)
{
	if (m_futureActions.empty()) {
		buildPath(state, map, history);
	}

	const Direction action = m_futureActions.back();
	m_futureActions.pop_back();
	return action;
}

void ExhaustiveSearch::buildPath(const GameState& state, const Map& map, const GameHistory& history)
{
	const PlayerState& baseState = state.getCurrent();
	StateCache cache(baseState);

	PlayerState bestFinalState;
	int numBestFinalSteps = std::numeric_limits<int>::max();

	// populate cache with a decent path that determines a better lower limit
	if (m_preconditioner) {
		GameState currentState = state;
		PlayerState& currentPlayerState = currentState.getCurrent();
		MOVE_RESULT r;
		int steps;
		do {
			PlayerState nextPlayerState = currentPlayerState;
			r = advance(nextPlayerState, m_preconditioner->getAction(currentState, map, history), map);
			// we assume that the simpler bot does not produce loops
			steps = *cache.add(nextPlayerState, currentPlayerState);
			currentPlayerState = nextPlayerState;
		} while (r != MOVE_RESULT::PASSED_FINAL_GOAL && r != MOVE_RESULT::INVALID);

		if (r == MOVE_RESULT::PASSED_FINAL_GOAL) {
			bestFinalState = currentPlayerState;
			numBestFinalSteps = steps;
		}
	}

	std::vector<PlayerState> statesToCheck { baseState };
	size_t numStatesChecked = 0;
	size_t numStateImprovements = 0;

	while (!statesToCheck.empty()) {
		const PlayerState currentState = statesToCheck.back();
		statesToCheck.pop_back();

		for (const Direction& action : ACTIONS) {
			++numStatesChecked;
			if (numStatesChecked % 1000000 == 0) {
				std::cout << std::format("Checked: {}, Improvements: {}, Ratio: {}, Best: {},\n",
					numStatesChecked,
					numStateImprovements,
					numStateImprovements / static_cast<double>(numStatesChecked),
					numBestFinalSteps);
			}

			PlayerState nextState = currentState;
			const MOVE_RESULT r = advance(nextState, action, map);
			if (r == MOVE_RESULT::INVALID) {
				// skip action
				continue;
			}

			if (std::optional<int> steps = cache.add(nextState, currentState)) {
				++numStateImprovements;
				if (r == MOVE_RESULT::PASSED_FINAL_GOAL) {
					if (*steps < numBestFinalSteps) {
						numBestFinalSteps = *steps;
						bestFinalState = nextState;
					}
					// finalStates.push_back(nextState);
					//  no need to try the other actions because they can't be better than this
					break;
				}
				// only consider this state if it could improve on the current best
				if (*steps < numBestFinalSteps)
					statesToCheck.push_back(nextState);
			}
		}
	}

	if (numBestFinalSteps == std::numeric_limits<int>::max()) {
		m_futureActions.push_back(ACTIONS[1]);
	}

	/*	const PlayerState& bestPathEnd = *std::min_element(finalStates.begin(), finalStates.end(), [&cache](const PlayerState& lhs, const PlayerState& rhs) {
			return cache.at(lhs).steps < cache.at(rhs).steps;
		});*/

	const PlayerState* pathState = &bestFinalState;
	const StateCache::PathInfo* info = nullptr;
	while (true) {
		info = &cache.at(*pathState);
		// reconstruct action
		m_futureActions.push_back(pathState->velocity - info->previous.velocity);
		if (info->steps == 0)
			break;
		pathState = &info->previous;
	}
}

}
