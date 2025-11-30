#ifndef GEO_SHAPES_HH
#define GEO_SHAPES_HH

#include <memory>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "point.hh"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

namespace geo {

namespace shape {

using Polygon = std::vector<Point>;

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

    [[nodiscard]] Shape polygon() const;
};

}

class Shape {
public:
    using Shapes = std::variant<shape::Polygon, shape::Circle, shape::Capsule, shape::Line>;

    Shape() : shape_({}) {}

    template <class T,
            class = std::enable_if_t<std::variant_size_v<Shapes> &&
                    std::is_constructible_v<Shapes, T>>>
    Shape(T&& v) : shape_(std::forward<T>(v)) {}

    static Shape Polygon(std::vector<geo::Point> const& points) { return shape::Polygon { points }; }
    static Shape Polygon(std::initializer_list<geo::Point> const& points) { return shape::Polygon { points }; } static Shape Circle(Point const& p, float radius) { return shape::Circle { p, radius }; }
    static Shape Capsule(Point const& p1, Point const& p2, float radius) { return shape::Capsule { p1, p2, radius }; }
    static Shape Line(Point const& p1, Point const& p2) { return shape::Line { p1, p2 }; }
    static Shape Line(float x1, float y1, float x2, float y2) { return shape::Line { x1, y1, x2, y2 }; }
    static Shape ThickLine(geo::Point const& p1, geo::Point const& p2, float width);
    static Shape Box(Point const& p, Point const& sz) { return Polygon({  p, { p.x, p.y + sz.y }, p + sz, { p.x + sz.x, p.y } }); }

    operator const Shapes&() const { return shape_; }
    operator Shapes&() { return shape_; }

    const Shapes& for_visit() const { return shape_; }
    Shapes& for_visit() { return shape_; }

    [[nodiscard]] bool contains_point(Point const& point) const;
    [[nodiscard]] Point center() const;
    [[nodiscard]] Bounds aabb() const;
    [[nodiscard]] bool aabb_intersects(Bounds const& bounds) const;

    static std::vector<Shape> voronoi(std::vector<Point> const& points, bool relax=false);
    static std::pair<std::vector<std::unique_ptr<Shape>>, std::unordered_map<Shape*, std::vector<Shape*>>>
        voronoi_with_neighbours(std::vector<Point> const& points, bool relax=false);

private:
    Shapes shape_;
};

}

#endif //TOPDOWN_SHAPES_HH
