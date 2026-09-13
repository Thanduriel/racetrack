#include "bots/snail.hpp"
#include "bots/rollinghorizon.hpp"
#include "game/game.hpp"

#ifdef WITH_GRAPHICS
#include "graphics/renderer.hpp"
#include "graphics/uibot.hpp"
#endif

int main()
{
    game::Map map("tracks/loop01.png");

    std::vector<std::unique_ptr<game::PlayerController>> players;
    players.emplace_back(std::make_unique<bots::Snail>(0x632fa1b4));
    players.emplace_back(std::make_unique<bots::RollingHorizon>(5));

#ifdef WITH_GRAPHICS
    graphics::Renderer renderer(map, 8.f);
    players.emplace_back(std::make_unique<bots::UIBot>(renderer));
#endif

    game::Racetrack game(map, std::move(players));

    game.run();

    return 0;
}
