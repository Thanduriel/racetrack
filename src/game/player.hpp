#pragma once

#include "../math/commontypes.hpp"
#include "map.hpp"

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace game {

using GameHistory = std::vector<Line>;

struct PlayerState {
    Point position;
    Direction velocity;
    int goal; //< next goal that has to be passed
    bool active;

    LineSegment step(Direction acceleration);
};

enum struct MOVE_RESULT {
    PASSED_FINAL_GOAL,
    PASSED_GOAL,
    INVALID,
    OK
};

// advance player step and check rules
MOVE_RESULT advance(PlayerState& state, Direction action, const Map& map);


struct GameState {
    std::vector<PlayerState> playerStates;
    size_t currentPlayer;

    PlayerState& getCurrent();
    const PlayerState& getCurrent() const;
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
    virtual std::string_view name() = 0;
    virtual Direction getAction(const GameState& state, const Map& map, const GameHistory& history) = 0;
};

}
