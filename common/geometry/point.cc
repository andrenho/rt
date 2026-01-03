#include "point.hh"

#include <algorithm>
#include <cassert>

#define TPH_POISSON_IMPLEMENTATION
#include "tph_poisson.h"
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

std::vector<geo::Point> Point::poisson(struct Bounds const& bounds, float radius, uint64_t seed, uint32_t max_attemps)
{
    const tph_poisson_real bounds_min[2] = { bounds.top_left.x, bounds.top_left.y };
    const tph_poisson_real bounds_max[2] = { bounds.bottom_right.x, bounds.bottom_right.y };

    const tph_poisson_args args = {
            .bounds_min = bounds_min,
            .bounds_max = bounds_max,
            .seed = seed,
            .radius = radius * 2,
            .ndims = INT32_C(2),
            .max_sample_attempts = max_attemps
    };

    tph_poisson_sampling sampling {};

    const int ret = tph_poisson_create(&args, nullptr, &sampling);
    if (ret != TPH_POISSON_SUCCESS)
        throw std::runtime_error("Failed creating Poisson sampling! Error code: " + std::to_string(ret));

    const tph_poisson_real *samples = tph_poisson_get_samples(&sampling);

    std::vector<geo::Point> points; points.reserve(sampling.nsamples);
    for (size_t i = 0; i < sampling.nsamples; ++i)
        points.emplace_back(samples[i*2], samples[i*2+1]);

    tph_poisson_destroy(&sampling);

    return points;
}

std::vector<geo::Point> Point::closest_points(std::vector<geo::Point> const& points, Point const& center)
{
    return {};
}

}