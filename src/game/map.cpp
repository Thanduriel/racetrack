#include "map.hpp"

#include <lodepng.h>

#include <algorithm>
#include <deque>
#include <format>
#include <iostream>
#include <map>
#include <optional>

namespace game {

using Color = unsigned char;
constexpr Color OUTSIDE_COLOR = 0;
constexpr Color MAX_OUTSIDE_COLOR = 75;
constexpr Color LANE_COLOR = 255;

bool Line::intersect(const LineSegment& segment) const
{
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        if (math::segmentsIntersect({ points[i], points[i + 1] }, segment))
            return true;
    }

    return false;
}

void Line::optimize()
{
    std::vector<Point> pointsOptimized;
    pointsOptimized.reserve(points.size());
    // first point is always needed
    pointsOptimized.push_back(points.front());

    // check if subsequent points lie on a line
    for (size_t i = 2; i < points.size(); ++i) {
        if (!math::onSegment({ pointsOptimized.back(), points[i] }, points[i - 1])) {
            pointsOptimized.push_back(points[i-1]);
        }
    }

    // last point
    pointsOptimized.push_back(points.back());

 //   std::cout << std::format("{} -> {}\n", points.size(), pointsOptimized.size());
    pointsOptimized.shrink_to_fit();
    points = std::move(pointsOptimized);
}

class PixelMap {
public:
    unsigned w, h;
    std::vector<unsigned char> buffer;

    PixelMap(const std::string& fileName)
    {
        const unsigned error = lodepng::decode(buffer, w, h, fileName, LodePNGColorType::LCT_GREY, 8);
        if (error) {
            std::cerr << "[Error] Could not load map. Decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            std::abort();
        }
    }

    void save(const std::string& fileName)
    {
        lodepng::State state;
        lodepng::encode(fileName, buffer, w, h, LodePNGColorType::LCT_GREY, 8);
    }

    size_t index(unsigned x, unsigned y) const
    {
        return x + w * y;
    }

    Color get(unsigned x, unsigned y) const
    {
        return buffer[index(x, y)];
    }

    void set(unsigned x, unsigned y, Color c)
    {
        buffer[index(x, y)] = c;
    }

    std::pair<unsigned, unsigned> clampRangeX(unsigned x, unsigned offset) const
    {
        const unsigned xMin = x > offset ? x - offset : 0;
        const unsigned xMax = x + offset < w ? x + offset : w;

        return { xMin, xMax };
    }

    std::pair<unsigned, unsigned> clampRangeY(unsigned y, unsigned offset) const
    {
        const unsigned yMin = y > offset ? y - offset : 0;
        const unsigned yMax = y + offset < h ? y + offset : h;

        return { yMin, yMax };
    }

    template <typename Pred>
    std::optional<Point> findNeighborhood(unsigned x, unsigned y, Pred pred) const
    {
        const auto [xMin, xMax] = clampRangeX(x, 1);
        const auto [yMin, yMax] = clampRangeY(y, 1);

#define CHECK_RETURN(xx, yy)       \
    if (pred(xx, yy, get(xx, yy))) \
    return std::make_optional<Point>(xx, yy)
        // prioritize adjacent pixels
        CHECK_RETURN(xMin, y);
        CHECK_RETURN(xMax, y);
        CHECK_RETURN(x, yMin);
        CHECK_RETURN(x, yMax);

        // diagonals
        CHECK_RETURN(xMin, yMin);
        CHECK_RETURN(xMax, yMin);
        CHECK_RETURN(xMin, yMax);
        CHECK_RETURN(xMax, yMax);

        return std::nullopt;
    }

    std::optional<Point> findNeighborhood(unsigned x, unsigned y, Color c) const
    {
        return findNeighborhood(x, y, [c](unsigned, unsigned, Color c2) {
            return c == c2;
        });
    }
};

Line makeLine(std::vector<Point> points)
{
    std::vector<Point> remainingPoints = std::move(points);

    // use deque because we build chains starting from a random point
    std::deque<Point> pixelChain { remainingPoints.back() };
    remainingPoints.pop_back();

    std::vector<IType> frontDists;
    std::vector<IType> backDists;

    auto updateDists = [&](Point o, std::vector<IType>& dists) {
        dists.clear();
        for (Point p : remainingPoints)
            dists.push_back(math::distSq(o, p));
    };

    while (!remainingPoints.empty()) {
        updateDists(pixelChain.front(), frontDists);
        updateDists(pixelChain.back(), backDists);

        auto frontDistIt = std::min_element(frontDists.begin(), frontDists.end());
        auto backDistIt = std::min_element(backDists.begin(), backDists.end());

        size_t idx = 0;
        if (*frontDistIt <= *backDistIt) {
            idx = std::distance(frontDists.begin(), frontDistIt);
            pixelChain.push_front(remainingPoints[idx]);
        } else {
            idx = std::distance(backDists.begin(), backDistIt);
            pixelChain.push_back(remainingPoints[idx]);
        }

        // remove pixel by moving it to the end since the order does not matter
        remainingPoints[idx] = remainingPoints.back();
        remainingPoints.pop_back();
    }

    return Line { std::vector<Point>(pixelChain.begin(), pixelChain.end()) };
}

Line traceBoundary(PixelMap& pixels, Point start)
{
    std::vector<Point> points;
    points.emplace_back(start);

    while (true) {
        Point p = points.back();

        pixels.set(p.x, p.y, MAX_OUTSIDE_COLOR);

        auto nextPoint = pixels.findNeighborhood(p.x, p.y, [&](unsigned ix, unsigned iy, Color c) {
            if (c == OUTSIDE_COLOR) {
                const auto [ixMin, ixMax] = pixels.clampRangeX(ix, 1);
                const auto [iyMin, iyMax] = pixels.clampRangeY(iy, 1);
                // check 4-neighborhood for lane
                return (pixels.get(ixMin, iy) > MAX_OUTSIDE_COLOR
                    || pixels.get(ixMax, iy) > MAX_OUTSIDE_COLOR
                    || pixels.get(ix, iyMin) > MAX_OUTSIDE_COLOR
                    || pixels.get(ix, iyMax) > MAX_OUTSIDE_COLOR);
            }

            return false;
        });

        if (!nextPoint) {
            break;
        }

        points.push_back(*nextPoint);
    }

    return Line { std::move(points) };
}

Map::Map(const std::string& mapFilePath)
    : width(0)
    , height(0)
{
    PixelMap pixels(mapFilePath);

    width = pixels.w;
    height = pixels.h;

    // use map because in the end we sort the goals by color
    std::map<unsigned char, std::vector<Point>> col_to_goals;

    for (unsigned y = 0; y < pixels.h; ++y) {
        for (unsigned x = 0; x < pixels.w; ++x) {
            const unsigned char c = pixels.get(x, y);
            if (c > MAX_OUTSIDE_COLOR && c != LANE_COLOR) {
                col_to_goals[c].push_back(Point { static_cast<IType>(x), static_cast<IType>(y) });
            }
        }
    }

    goals.reserve(col_to_goals.size());
    for (auto& [c, points] : col_to_goals) {
        goals.emplace_back(makeLine(points));
    }
    for (Line& line : goals){
        line.optimize();
    }

    if (goals.size() < 2) {
        std::cerr << "[Error] Track has less than two goals.\n";
        std::abort();
    } else {
        std::cout << std::format("Found {} goals.\n", goals.size());
    }

    // first goal is start and finish
    start = goals.front();
    goals.emplace_back(std::move(goals.front()));
    goals.erase(goals.begin());

    const Line& start = goals.front();
    const auto boundary0Begin = pixels.findNeighborhood(start.points.front().x, start.points.front().y, OUTSIDE_COLOR);
    if (!boundary0Begin) {
        std::cerr << "[Error] Could not find the 1st boundary.\n";
        std::abort();
    }
    boundary0 = traceBoundary(pixels, *boundary0Begin);
    boundary0.optimize();

    const auto boundary1Begin = pixels.findNeighborhood(start.points.back().x, start.points.back().y, OUTSIDE_COLOR);
    if (!boundary1Begin) {
        std::cerr << "[Error] Could not find the 2nd boundary.\n";
        std::abort();
    }
    boundary1 = traceBoundary(pixels, *boundary1Begin);
    boundary1.optimize();
    //pixels.save("debug.png");

    std::cout << std::format("Boundarys have lengths {} and {}.\n", boundary0.points.size(), boundary1.points.size());
}

bool Map::intersectBoundary(const LineSegment& segment) const
{
    return boundary0.intersect(segment) || boundary1.intersect(segment);
}
}
