#include "player.hpp"

namespace game {

LineSegment PlayerState::step(Direction acceleration)
{
	Point prevPos = position;
	velocity += acceleration;
	position += velocity;

	return { prevPos, position };
}

bool operator==(const PlayerState& a, const PlayerState& b)
{
	return a.position == b.position
		&& a.velocity == b.velocity
		&& a.goal == b.goal
		&& a.active && b.active;
}

MOVE_RESULT advance(PlayerState& state, Direction action, const Map& map)
{
	const LineSegment seg = state.step(action);
	if (map.intersectBoundary(seg)) {
		state.active = false;
		return MOVE_RESULT::INVALID;
	}

	MOVE_RESULT result = MOVE_RESULT::OK;
	for (; state.goal < static_cast<int>(map.goals.size()); ++state.goal) {
		if (!map.goals[state.goal].intersect(seg)) {
			break;
		}
		result = MOVE_RESULT::PASSED_GOAL;
	}

	if (state.goal >= static_cast<int>(map.goals.size())) {
		result = MOVE_RESULT::PASSED_FINAL_GOAL;
	}

	return result;
}

}
