#ifndef TOPDOWN_SHAPES_HH
#define TOPDOWN_SHAPES_HH

#include <utility>
#include <variant>
#include <vector>

#include "point.hh"

namespace geo {

using Polygon = std::vector<Point>;

inline Polygon Box(Point const& p, Point const& sz)
{
    return { p, { p.x, p.y + sz.y }, p + sz, { p.x + sz.x, p.y } };
}

struct Circle {
    Point center;
    float  radius;
};

using Shape = std::variant<Polygon, Circle>;

}

#endif //TOPDOWN_SHAPES_HH
