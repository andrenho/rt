#ifndef GEO_SHAPES_HH
#define GEO_SHAPES_HH

#include <utility>
#include <variant>
#include <vector>

#include "point.hh"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

namespace geo {

class Polygon : public std::vector<Point> {
public:
    using std::vector<Point>::vector;

    [[nodiscard]] Point center() const;
};

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
