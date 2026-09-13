#pragma once

#include "../math/commontypes.hpp"
#include "map.hpp"

#include <array>
#include <string>
#include <vector>

namespace game {

using GameHistory = std::vector<Line>;

struct PlayerState {
    Point position;
    Direction velocity;
    int goal; //< next goal that has to be passed
    bool active;
};

struct GameState {
    std::vector<PlayerState> playerStates;
    size_t activePlayer;

    PlayerState& getActive();
    const PlayerState& getActive() const;
};

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
    virtual Direction getAction(const GameState& state, const Map& map, const GameHistory& history) = 0;
};

}
