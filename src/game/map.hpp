#pragma once

#include "../math/commontypes.hpp"

#include <string>
#include <vector>

namespace game {
// line on the lattice described by a sequence of points
struct Line {
    std::vector<Point> points;

	bool intersect(const LineSegment& segment) const;
};

// the map or actual racetrack
class Map {
public:
    Map(const std::string& mapFilePath);

	bool intersectBoundary(const LineSegment& segment) const;

    unsigned width;
    unsigned height;

    Line boundary0;
    Line boundary1;

	Line start;
    std::vector<Line> goals;
};
}
