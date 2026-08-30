#include "game/game.hpp"

int main()
{
	game::Racetrack game("tracks/loop01.png", 1);

	game.run();

	return 0;
}
