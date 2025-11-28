#include "shapes.hh"

#include <cassert>
#include <numbers>
#include <unordered_map>

#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"

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

std::vector<Shape> Shape::voronoi(std::vector<Point> const& pts, bool relax)
{
    std::vector<Shape> shapes;
    std::vector<Point> points = relax ? Point::relax_grid(pts) : pts;

    jcv_diagram diagram {};
    std::vector<jcv_point> jcv_points;
    jcv_points.reserve(points.size());
    for (auto const& p: points)
        jcv_points.emplace_back(p.x, p.y);

    jcv_diagram_generate((int) jcv_points.size(), jcv_points.data(), nullptr, nullptr, &diagram);
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);

    for(int i = 0; i < diagram.numsites; ++i) {
        std::vector<geo::Point> ppoints;

        const jcv_site* site = &sites[i];

        const jcv_graphedge* e = site->edges;
        while (e) {
            ppoints.emplace_back(e->pos[0].x, e->pos[0].y);
            e = e->next;
        }

        shapes.emplace_back(Shape::Polygon(ppoints));
    }

    jcv_diagram_free(&diagram);

    // TODO - relax grid

    return shapes;
}

std::pair<std::vector<std::unique_ptr<Shape>>, std::unordered_map<Shape*, std::vector<Shape*>>>
Shape::voronoi_with_neighbours(std::vector<Point> const& pts, bool relax)
{
    std::vector<std::unique_ptr<Shape>> shapes;
    std::unordered_map<Shape*, std::vector<Shape*>> shape_neighbours;
    std::vector<Point> points = relax ? Point::relax_grid(pts) : pts;

    jcv_diagram diagram {};
    std::vector<jcv_point> jcv_points;
    jcv_points.reserve(points.size());
    for (auto const& p: points)
        jcv_points.emplace_back(p.x, p.y);

    jcv_diagram_generate((int) jcv_points.size(), jcv_points.data(), nullptr, nullptr, &diagram);
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);

    std::unordered_map<Shape*, std::vector<jcv_site*>> shape_neighbour_sites;
    std::unordered_map<jcv_site const*, Shape*> sites_shapes;

    for(int i = 0; i < diagram.numsites; ++i) {
        std::vector<geo::Point> ppoints;
        std::vector<jcv_site*> neighbours;

        const jcv_site* site = &sites[i];

        const jcv_graphedge* e = site->edges;
        while (e) {
            ppoints.emplace_back(e->pos[0].x, e->pos[0].y);
            neighbours.emplace_back(e->neighbor);
            e = e->next;
        }

        auto& shape = shapes.emplace_back(std::make_unique<Shape>(Shape::Polygon(ppoints)));
        shape_neighbour_sites[shape.get()] = std::move(neighbours);
        sites_shapes[site] = shape.get();
    }

    jcv_diagram_free(&diagram);

    // find neighbours
    for (auto& shape: shapes)
        for (jcv_site* site: shape_neighbour_sites.at(shape.get()))
            if (site)
                shape_neighbours[shape.get()].emplace_back(sites_shapes.at(site));

    // TODO - relax grid

    return { std::move(shapes), std::move(shape_neighbours) };
}

namespace shape {

Shape Capsule::polygon() const
{
    return Shape::ThickLine(p1, p2, radius);
}

}

}