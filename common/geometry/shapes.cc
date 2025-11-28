#include "shapes.hh"

#include <cassert>
#include <numbers>

namespace geo {

bool Shape::contains_point(Point const& p) const
{
    return std::visit([&](auto const& s) -> bool {
        using T = std::decay_t<decltype(s)>;

        if constexpr (std::is_same_v<T, shape::Polygon>) {
            bool inside = false;
            size_t const n = s.size();
            if (n < 3) return false;

            for (size_t i = 0, j = n - 1; i < n; j = i++)
            {
                Point const& a = s[j];
                Point const& b = s[i];

                bool intersect = ((a.y > p.y) != (b.y > p.y)) &&
                        (p.x < (b.x - a.x) * (p.y - a.y) /
                                (b.y - a.y) + a.x);
                if (intersect) inside = !inside;
            }
            return inside;
        } else if constexpr (std::is_same_v<T, shape::Circle>) {
            float dx = p.x - s.center.x;
            float dy = p.y - s.center.y;
            return dx * dx + dy * dy <= s.radius * s.radius;
        } else if constexpr (std::is_same_v<T, shape::Capsule>) {
            return (Shape::Circle(s.p1, s.radius).contains_point(p) ||
                    Shape::Circle(s.p2, s.radius).contains_point(p) ||
                    Shape { ThickLine(s.p1, s.p2, s.radius) }.contains_point(p));
        } else {
            return false;
        }
    }, for_visit());
}

Shape Shape::ThickLine(Point const& p1, Point const& p2, float width)
{
    Point dir = p2 - p1;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    // unit perpendicular to the line (rotate dir by +90 degrees and normalize)
    Point perp = { -dir.y / len, dir.x / len };
    Point offs = perp * width;

    // rectangle corners in CCW order (p1+off, p1-off, p2-off, p2+off)
    return Shape::Polygon({ p1 + offs, p1 - offs, p2 - offs, p2 + offs });
}

Point Shape::center() const
{
    return std::visit(overloaded {
        [&](shape::Polygon const& poly) {
            float x = 0.f, y = 0.f;
            for (Point const& p: poly) {
                x += p.x;
                y += p.y;
            }
            return geo::Point { x / (float) poly.size(), y / (float) poly.size() };
        },
        [&](shape::Circle const& c) {
            return c.center;
        },
        [&](shape::Line const& ln) {
            return geo::Point { (ln.p1.x + ln.p2.x) / 2.f, (ln.p1.y + ln.p2.y) / 2.f };
        },
        [&](shape::Capsule const& c) {
            return c.polygon().center();
        },
    }, for_visit());
}

Bounds Shape::aabb() const
{
    return std::visit(overloaded {
            [&](shape::Polygon const& poly) {
                float min_x = std::numeric_limits<float>::max(), min_y = std::numeric_limits<float>::max();
                float max_x = std::numeric_limits<float>::min(), max_y = std::numeric_limits<float>::min();
                for (Point const& p: poly) {
                    min_x = std::min(min_x, p.x);
                    min_y = std::min(min_y, p.y);
                    max_x = std::min(max_x, p.x);
                    max_y = std::min(max_y, p.y);
                }
                assert(max_x >= min_x && max_y >= min_y);
                return Bounds({ min_x, min_y }, { max_x, max_y });
            },
            [&](shape::Circle const& c) {
                return Bounds {
                    { c.center.x - c.radius, c.center.y + c.radius },
                    { c.center.x + c.radius, c.center.y + c.radius },
                };
            },
            [&](shape::Line const& ln) {
                return Bounds {
                    { std::min(ln.p1.x, ln.p2.x), std::min(ln.p1.x, ln.p2.x) },
                    { std::max(ln.p1.x, ln.p2.x), std::max(ln.p1.x, ln.p2.x) }
                };
            },
            [&](shape::Capsule const& c) {
                Shape s1 = Shape::Circle(c.p1, c.radius);
                Shape s2 = Shape::Circle(c.p2, c.radius);
                Shape s3 = Shape::ThickLine(c.p1, c.p2, c.radius);
                return Bounds { {
                    std::min(std::min(s1.aabb().top_left.x, s2.aabb().top_left.x), s3.aabb().top_left.x),
                    std::min(std::min(s1.aabb().top_left.y, s2.aabb().top_left.y), s3.aabb().top_left.y),
                }, {
                    std::max(std::max(s1.aabb().bottom_right.x, s2.aabb().bottom_right.x), s3.aabb().bottom_right.x),
                    std::max(std::max(s1.aabb().bottom_right.y, s2.aabb().bottom_right.y), s3.aabb().bottom_right.y),
                } };
            },
    }, for_visit());
}

namespace shape {

Shape Capsule::polygon() const
{
    return Shape::ThickLine(p1, p2, radius);
}

}

}