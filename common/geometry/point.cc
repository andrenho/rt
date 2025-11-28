#include "point.hh"

#include "shapes.hh"

namespace geo {

Point::operator struct Size() const
{
    return { x, y };
}

std::vector<geo::Point> Point::generate_random_points(Shape const& area, float density, float randomness, bool relax, std::mt19937& rng)
{
    (void) area; (void) density; (void) randomness; (void) relax; (void) rng;
    return {};
}

}