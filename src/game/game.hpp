#pragma once

#include "player.hpp"
#include "map.hpp"

#include <string>
#include <vector>
#include <memory>

namespace game {

class Racetrack {
public:
    Racetrack(const Map& map, std::vector<std::unique_ptr<PlayerController>> players);

	void step();
    void run();

private:
	const Map& m_map;
	std::vector<std::unique_ptr<PlayerController>> m_players;
	GameState m_state;
	GameHistory m_history;
};

}
