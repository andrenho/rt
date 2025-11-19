#include "shapes.hh"

geo::Point geo::Polygon::center() const
{
    float x = 0.f, y = 0.f;
    for (Point const& p: *this) {
        x += p.x;
        y += p.y;
    }
    return { x / (float) size(), y / (float) size() };
}
