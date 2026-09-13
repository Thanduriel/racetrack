#pragma once

#include "../game/player.hpp"

#include <random>

namespace bots {

// Constant speed of 1 to make crashes impossible.
// Random walk with preference for the direction of the next goal.
class Snail : public game::PlayerController {
public:
	Snail(uint32_t seed);

    std::string name() override;
    Direction getAction(const game::GameState& state, const game::Map& map, const game::GameHistory& history) override;

private:
	int m_prevGoal;
	Point m_currentDestination;
	std::minstd_rand m_rng;
};
};
