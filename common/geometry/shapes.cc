#include "shapes.hh"

namespace geo {

Point Polygon::center() const
{
    float x = 0.f, y = 0.f;
    for (Point const& p: *this) {
        x += p.x;
        y += p.y;
    }
    return { x / (float) size(), y / (float) size() };
}

bool contains_point(Shape const& shape, Point const& p)
{
    return std::visit([&](auto const& s) -> bool {
        using T = std::decay_t<decltype(s)>;

        if constexpr (std::is_same_v<T, Polygon>) {
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
        } else if constexpr (std::is_same_v<T, Circle>) {
            float dx = p.x - s.center.x;
            float dy = p.y - s.center.y;
            return dx * dx + dy * dy <= s.radius * s.radius;
        } else if constexpr (std::is_same_v<T, Capsule>) {
            // TODO - check central rectangle
            return contains_point(Circle { s.p1, s.radius }, p) ||
                   contains_point(Circle { s.p2, s.radius }, p) ||
                   contains_point(ThickLine(s.p1, s.p2, s.radius), p);
        } else {
            return false;
        }
    }, shape);
}

Polygon ThickLine(Point const& p1, Point const& p2, float width)
{
    Point dir = p2 - p1;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    // unit perpendicular to the line (rotate dir by +90 degrees and normalize)
    Point perp = { -dir.y / len, dir.x / len };
    float half = width * 0.5f;
    Point offs = perp * half;

    // rectangle corners in CCW order (p1+off, p1-off, p2-off, p2+off)
    return Polygon { p1 + offs, p1 - offs, p2 - offs, p2 + offs };
}

}