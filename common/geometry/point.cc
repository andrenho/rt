#include "point.hh"

#include <algorithm>
#include <cassert>

#include "shapes.hh"

namespace geo {

Point::operator struct Size() const
{
    return { x, y };
}

//
// POINT GENERATION
//

static std::vector<geo::Point>& remove_points_not_in_shape(std::vector<geo::Point>& points, Shape const& area)
{
    points.erase(
            std::remove_if(points.begin(), points.end(), [&](geo::Point const& p) { return !area.contains_point(p); }),
            points.end());
    return points;
}

std::vector<geo::Point> Point::grid(Bounds const& bounds, float avg_point_distance)
{
    std::vector<geo::Point> points;

    for (float x = bounds.top_left.x; x < bounds.bottom_right.x; x += avg_point_distance) {
        for (float y = bounds.top_left.y; y < bounds.bottom_right.y; y += avg_point_distance) {
            points.emplace_back(x, y);
        }
    }

    return points;
}

std::vector<geo::Point> Point::grid(Shape const& area, float avg_point_distance)
{
    Bounds bounds = area.aabb();
    std::vector<geo::Point> points = grid(bounds, avg_point_distance);
    return remove_points_not_in_shape(points, area);
}

std::vector<geo::Point> Point::grid(Bounds const& bounds, float avg_point_distance, std::mt19937& rng, float randomness)
{
    assert(randomness >= 0.f && randomness <= 1.f);

    std::vector<geo::Point> points = grid(bounds, avg_point_distance);

    std::uniform_real_distribution<float> distances(0.0, avg_point_distance * randomness);
    std::uniform_real_distribution<float> angles(0.0, 2.0);
    for (auto& p: points) {
        p.x += distances(rng) * (float) cos(angles(rng));
        p.y += distances(rng) * (float) sin(angles(rng));
    }

    return points;
}

std::vector<geo::Point> Point::grid(Shape const& area, float avg_point_distance, std::mt19937& rng, float randomness)
{
    Bounds bounds = area.aabb();
    auto points = grid(bounds, avg_point_distance, rng, randomness);
    return remove_points_not_in_shape(points, area);
}

}