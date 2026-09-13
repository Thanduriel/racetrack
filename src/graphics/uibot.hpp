#pragma once

#include "../game/player.hpp"
#include "renderer.hpp"

namespace bots {
class UIBot : public game::PlayerController {
public:
    UIBot(graphics::Renderer& renderer);

    std::string name() override;
    Direction getAction(const game::GameState& state, const game::Map& map, const game::GameHistory& history) override;

private:
    graphics::Renderer& m_renderer;
};
}
