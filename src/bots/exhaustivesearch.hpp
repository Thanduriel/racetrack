#pragma once

#include "../game/player.hpp"

#include <iostream>
#include <optional>
#include <random>
#include <unordered_map>
#include <memory>

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

template <>
struct std::hash<game::PlayerState> {
	std::size_t operator()(const game::PlayerState& s) const
	{
		std::size_t ret = 0;
		hash_combine(ret, s.position.x, s.position.y, s.velocity.x, s.velocity.y, s.goal);
		return ret;
	}
};

namespace bots {

class StateCache {
public:
	struct PathInfo {
		int steps;
		game::PlayerState previous;
	};

	StateCache(const game::PlayerState& baseState);

	// @return Number of steps in path if the current move was added.
	std::optional<int> add(const game::PlayerState& state, const game::PlayerState& prevState);

	const PathInfo& at(const game::PlayerState& state) const;

	int longestPath() const;

private:
	std::unordered_map<game::PlayerState, PathInfo> m_cache;
};

class ExhaustiveSearch : public game::PlayerController {
public:
	ExhaustiveSearch(std::unique_ptr<game::PlayerController> preconditioner = nullptr);

	std::string_view name() override;
	Direction getAction(const game::GameState& state, const game::Map& map, const game::GameHistory& history) override;

private:
	void buildPath(const game::GameState& state, const game::Map& map, const game::GameHistory& history);

	std::vector<Direction> m_futureActions;
	std::unique_ptr<game::PlayerController> m_preconditioner;
};

}
