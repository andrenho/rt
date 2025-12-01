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

std::vector<geo::Point> Point::grid(Bounds const& bounds, float avg_point_distance_w, float avg_point_distance_h)
{
    std::vector<geo::Point> points;

    for (float x = bounds.top_left.x; x < bounds.bottom_right.x; x += avg_point_distance_w) {
        for (float y = bounds.top_left.y; y < bounds.bottom_right.y; y += avg_point_distance_h) {
            points.emplace_back(x, y);
        }
    }

    return points;
}

std::vector<geo::Point> Point::grid(Shape const& area, float avg_point_distance_w, float avg_point_distance_h)
{
    Bounds bounds = area.aabb();
    std::vector<geo::Point> points = grid(bounds, avg_point_distance_w, avg_point_distance_h);
    return remove_points_not_in_shape(points, area);
}

std::vector<geo::Point> Point::grid(Bounds const& bounds, float avg_point_distance_w, float avg_point_distance_h, std::mt19937& rng, float randomness)
{
    assert(randomness >= 0.f && randomness <= 1.f);

    std::vector<geo::Point> points = grid(bounds, avg_point_distance_w, avg_point_distance_h);

    std::uniform_real_distribution<float> distances_x(0.0, avg_point_distance_w * randomness);
    std::uniform_real_distribution<float> distances_y(0.0, avg_point_distance_h * randomness);
    std::uniform_real_distribution<float> angles(0.0, 2.0);
    for (auto& p: points) {
        p.x += distances_x(rng) * (float) cos(angles(rng));
        p.y += distances_y(rng) * (float) sin(angles(rng));
    }

    return points;
}

std::vector<geo::Point> Point::grid(Shape const& area, float avg_point_distance_w, float avg_point_distance_h, std::mt19937& rng, float randomness)
{
    Bounds bounds = area.aabb();
    auto points = grid(bounds, avg_point_distance_w, avg_point_distance_h, rng, randomness);
    return remove_points_not_in_shape(points, area);
}

std::vector<geo::Point> Point::relax_grid(std::vector<geo::Point> const& grid)
{
    auto polygons = Shape::voronoi(grid, false);

    std::vector<geo::Point> points;
    points.reserve(polygons.size());
    for (auto const& shape: polygons)
        points.emplace_back(shape.center());

    return points;
}

bool Point::operator==(Point const& other) const
{
    int x1 = static_cast<int>(std::round(x * 1000));
    int x2 = static_cast<int>(std::round(other.x * 1000));
    int y1 = static_cast<int>(std::round(y * 1000));
    int y2 = static_cast<int>(std::round(other.y * 1000));
    return x1 == x2 && y1 == y2;
}

bool Bounds::intersects(Bounds const& a) const
{
    // One rectangle is completely to the left of the other
    if (bottom_right.x < a.top_left.x || a.bottom_right.x < top_left.x)
        return false;

    // One rectangle is completely above the other
    if (bottom_right.y < a.top_left.y || a.bottom_right.y < top_left.y)
        return false;

    return true; // They overlap or touch
}

}