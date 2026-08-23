#pragma once

#include "../math/commontypes.hpp"

#include <vector>
#include <string>

namespace game
{
	// line on the lattice described by a sequence of points
	struct Line
	{
		std::vector<Point> points;
	};

	// the map or actual racetrack
	class Map
	{
	public:
		Map(const std::string &mapFilePath);

		unsigned width;
		unsigned height;

		Line border0;
		Line border1;

		std::vector<Line> goals;
	};
}
