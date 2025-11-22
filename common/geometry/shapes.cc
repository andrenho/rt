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

        if constexpr (std::is_same_v<T, Polygon>)
        {
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
        }
        else // Circle
        {
            float dx = p.x - s.center.x;
            float dy = p.y - s.center.y;
            return dx * dx + dy * dy <= s.radius * s.radius;
        }
    }, shape);
}

}