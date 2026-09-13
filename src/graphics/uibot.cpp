#include "uibot.hpp"

namespace bots {

UIBot::UIBot(graphics::Renderer& renderer)
    : m_renderer(renderer)
{
}

std::string UIBot::name()
{
    return "user-input";
}

Direction UIBot::getAction(const game::GameState& state, const game::Map& map, const game::GameHistory& history)
{
    return m_renderer.getAction(state, history);
}

}
