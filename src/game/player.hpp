#pragma once

#include "../math/commontypes.hpp"

#include <array>
#include <string>
#include <vector>

namespace game {

class Map;

struct PlayerState {
    Point position;
    Direction velocity;
    int goal; //< next goal that has to be passed
    bool active;
};

using GameState = std::vector<PlayerState>;

constexpr std::array<Direction, 9> ACTIONS = { { { 0, 0 },
    { 0, -1 },
    { 0, 1 },
    { -1, 0 },
    { 1, 0 },
    { -1, -1 },
    { 1, -1 },
    { -1, 1 },
    { 1, 1 } } };

/*
enum struct Action {
        NONE, BOT, TOP, LEFT, RIGHT, BOT_LEFT, BOT_RIGHT, TOP_LEFT, TOP_RIGHT, Count
};
constexpr Direction toDirection(Action action);*/

class PlayerController {
public:
    virtual ~PlayerController() = default;
    virtual std::string name() = 0;
    virtual Direction getAction(const GameState& state, const Map& map, size_t player) = 0;
};

}
