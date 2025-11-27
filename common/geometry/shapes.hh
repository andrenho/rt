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

Polygon ThickLine(geo::Point const& p1, geo::Point const& p2, float width);

struct Circle {
    Point center;
    float  radius;
};

struct Line {
    Line(Point const& p1_, Point const& p2_) : p1(p1_), p2(p2_) {}
    Line(float x1, float y1, float x2, float y2) : p1({ x1, y1 }), p2({ x2, y2 }) {}
    Point p1, p2;
};

struct Capsule {
    Point p1, p2;
    float radius;
};

using Shapes = std::variant<Polygon, Circle, Capsule, Line>;

class Shape {
public:
    Shape() : shape_({}) {}

    template <class T,
            class = std::enable_if_t<std::variant_size_v<Shapes> &&
                    std::is_constructible_v<Shapes, T>>>
    Shape(T&& v) : shape_(std::forward<T>(v)) {}

    operator const Shapes&() const { return shape_; }
    operator Shapes&() { return shape_; }

    const Shapes& for_visit() const { return shape_; }
    Shapes& for_visit() { return shape_; }

private:
    Shapes shape_;
};

bool contains_point(Shape const& shape, Point const& point);

}

#endif //TOPDOWN_SHAPES_HH
