#pragma once

#include "../game/player.hpp"
#include <deque>

namespace bots {
class RollingHorizon : public game::PlayerController {
public:
    RollingHorizon(int maxDepth);

    std::string name() override;
    Direction getAction(const game::GameState& state, const game::Map& map, const game::GameHistory& history) override;

private:
    int m_maxDepth;

    struct Node {
        //game::GameState state;
        game::PlayerState state;
        float distanceTraveled;
        int depth;
        const Node* parent;
        Direction action;
        //std::array<Node*, game::ACTIONS.size()> childs;
    };
    std::deque<Node> m_nodes;
    Node m_root;
};
} // namespace bots
