#include "rollinghorizon.hpp"

namespace bots {

using namespace game;

RollingHorizon::RollingHorizon(int maxDepth)
    : m_maxDepth(maxDepth)
{
}

std::string RollingHorizon::name() { return "RollingHorizon"; }

Direction RollingHorizon::getAction(const game::GameState& state, const game::Map& map, const game::GameHistory& history)
{
    m_nodes.clear();

    std::vector<const Node*> nodesToProcess;
    const Node* bestNode = nullptr;

    m_root = { state.getActive(), 0.f, 0, nullptr, Direction { 0, 0 } };
    nodesToProcess.push_back(&m_root);

    while (!nodesToProcess.empty()) {
        const Node& node = *nodesToProcess.back();
        nodesToProcess.pop_back();

        if (node.depth < m_maxDepth) {
            // explore all possible moves
            for (Direction action : ACTIONS) {
                PlayerState newState = node.state;
                const MOVE_RESULT result = advance(newState, action, map);

                if (result == MOVE_RESULT::INVALID) {
                    continue;
                }

                // too fast to stop before a crash
                if (newState.velocity.len() > static_cast<float>(m_maxDepth)) {
                    continue;
                }

                const float distTraveled = node.distanceTraveled + dist(newState.position, node.state.position);
                m_nodes.push_back(Node { newState, distTraveled, node.depth + 1, &node, action });
                nodesToProcess.push_back(&m_nodes.back());
            }
        } else {
            // heuristic for quality
            if (!bestNode
                || node.state.goal > bestNode->state.goal
                || node.distanceTraveled > bestNode->distanceTraveled) {
                bestNode = &node;
            }
        }
    }

    // every possible action leads to an invalid state
    if (!bestNode) {
        return { 0, 0 };
    }

    // reverse moves to get best action for current step
    while (bestNode->depth > 1) {
        bestNode = bestNode->parent;
    }

    return bestNode->action;
}
}
