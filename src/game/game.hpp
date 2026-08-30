#pragma once

#include "player.hpp"
#include "map.hpp"

#include <string>
#include <vector>
#include <memory>

namespace game {

class Racetrack {
public:
    Racetrack(const std::string& mapFilePath, int numPlayers);

    void run();

private:
	Map m_map;
	//std::vector<Player> m_players;
	std::vector<std::unique_ptr<PlayerController>> m_players;
	GameState m_playerStates;
};

}
