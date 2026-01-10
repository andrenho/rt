#include "shapes.hh"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <numbers>
#include <ranges>
#include <unordered_map>

#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"

namespace ranges = std::ranges;

namespace geo {

Shape Shape::Box(Point const& p, Point const& sz, float angle)
{
    // Unrotated corners
    Point c0 = p;
    Point c1 = { p.x,         p.y + sz.y };
    Point c2 = p + sz;
    Point c3 = { p.x + sz.x,  p.y };

    // Center of the box
    Point center = { p.x + sz.x * 0.5f, p.y + sz.y * 0.5f };

    float s = std::sin(angle);
    float c = std::cos(angle);

    auto rotate = [&](Point const& v) {
        Point d = v - center;
        return Point{
                center.x + d.x * c - d.y * s,
                center.y + d.x * s + d.y * c
        };
    };

    return Polygon({ rotate(c0), rotate(c1), rotate(c2), rotate(c3) });
}

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
                    max_x = std::max(max_x, p.x);
                    max_y = std::max(max_y, p.y);
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

bool Shape::aabb_intersects(Bounds const& bounds) const
{
    return aabb().intersects(bounds);
}

bool Shape::intersects(Shape const& other) const
{
    return std::visit(overloaded {
        [&](shape::Line const& ln)      {
            return std::visit(overloaded {
                [&](shape::Line const& oln)      { return segment_intersection(ln, oln); },
                [&](shape::Polygon const& opoly) { return ranges::any_of(shape::polygon_lines(opoly), [&](auto const& oln) { return segment_intersection(ln, oln); }); },
                [&](shape::Circle const& oc)     { return segment_circle_intersection(ln, oc); },
                [&](shape::Capsule const& oc)    { return ranges::any_of(oc.subshapes(), [&](auto const& oshp) { return intersects(oshp); }); }
            }, other.for_visit());
        },
        [&](shape::Polygon const& poly) {
            return std::visit(overloaded {
                [&](shape::Line const&)          { return other.intersects(*this); },
                [&](shape::Polygon const& opoly) { return polygon_polygon_intersection(poly, opoly); },
                [&](shape::Circle const& oc)     { return polygon_circle_intersection(poly, oc); },
                [&](shape::Capsule const& oc)    { return ranges::any_of(oc.subshapes(), [&](auto const& oshp) { return intersects(oshp); }); }
            }, other.for_visit());
        },
        [&](shape::Circle const& c)     {
            return std::visit(overloaded {
                [&](shape::Line const& oln)      { return segment_circle_intersection(oln, c); },
                [&](shape::Polygon const& opoly) { return polygon_circle_intersection(opoly, c); },
                [&](shape::Circle const& oc)     { return circle_circle_intersection(c, oc); },
                [&](shape::Capsule const& oc)    { return ranges::any_of(oc.subshapes(), [&](auto const& oshp) { return intersects(oshp); }); }
            }, other.for_visit());
        },
        [&](shape::Capsule const& c)    {
            return std::visit(overloaded {
                [&](shape::Line const& oln)      { return Shape::Line(oln.p1, oln.p2).intersects(*this); },
                [&](shape::Polygon const& opoly) { return Shape::Polygon(opoly).intersects(*this); },
                [&](shape::Circle const& oc)     { return Shape::Circle(oc.center, oc.radius).intersects(*this); },
                [&](shape::Capsule const&) {
                    for (auto const& shp1: c.subshapes())
                        for (auto const& shp2: c.subshapes())
                            if (shp1.intersects(shp2))
                                return true;
                    return false;
                },
            }, other.for_visit());
        }
    }, for_visit());
}

bool Shape::segment_intersection(shape::Line const& ln1, shape::Line const& ln2)
{
    constexpr double eps = 1e-12;

    auto orient = [&](const geo::Point& a, const geo::Point& b, const geo::Point& c) {
        return (b.x - a.x) * (c.y - a.y)
                - (b.y - a.y) * (c.x - a.x);
    };

    auto onSegment = [&](const geo::Point& a, const geo::Point& b, const geo::Point& p) {
        return p.x >= std::min(a.x, b.x) - eps &&
                p.x <= std::max(a.x, b.x) + eps &&
                p.y >= std::min(a.y, b.y) - eps &&
                p.y <= std::max(a.y, b.y) + eps;
    };

    double o1 = orient(ln1.p1, ln1.p2, ln2.p1);
    double o2 = orient(ln1.p1, ln1.p2, ln2.p2);
    double o3 = orient(ln2.p1, ln2.p2, ln1.p1);
    double o4 = orient(ln2.p1, ln2.p2, ln1.p2);

    // Proper intersection
    if (((o1 > eps && o2 < -eps) || (o1 < -eps && o2 > eps)) && ((o3 > eps && o4 < -eps) || (o3 < -eps && o4 > eps)))
        return true;

    // Colinear / touching cases
    if (std::abs(o1) <= eps && onSegment(ln1.p1, ln1.p2, ln2.p1)) return true;
    if (std::abs(o2) <= eps && onSegment(ln1.p1, ln1.p2, ln2.p2)) return true;
    if (std::abs(o3) <= eps && onSegment(ln2.p1, ln2.p2, ln1.p1)) return true;
    if (std::abs(o4) <= eps && onSegment(ln2.p1, ln2.p2, ln1.p2)) return true;

    return false;
}

bool Shape::segment_circle_intersection(shape::Line const& ln, shape::Circle const& c)
{
    // const Vec2& a, const Vec2& b, const Vec2& center, double r

    geo::Point ab = ln.p2 - ln.p1;
    geo::Point ac = c.center - ln.p1;

    double t = ac.dot(ab) / ab.dot(ab);
    t = std::max(0.0, std::min(1.0, t));

    geo::Point closest(ln.p1.x + ab.x * t, ln.p1.y + ab.y * t);

    return (closest - c.center).length_sq() <= c.radius * c.radius;
}

bool Shape::polygon_polygon_intersection(shape::Polygon const& p1, shape::Polygon const& p2)
{
    int n1 = p1.size();
    int n2 = p2.size();

    // 1. Edge–edge intersections
    for (int i = 0; i < n1; ++i) {
        shape::Line e1 { p1[i], p1[(i + 1) % n1] };

        for (int j = 0; j < n2; ++j) {
            shape::Line e2 { p2[j], p2[(j + 1) % n2] };
            if (segment_intersection(e1, e2))
                return true;
        }
    }

    // 2. Containment checks
    if (Shape::Polygon(p2).contains_point(p1[0]))
        return true;
    if (Shape::Polygon(p1).contains_point(p2[0]))
        return true;

    return false;
}

bool Shape::polygon_circle_intersection(shape::Polygon const& p, shape::Circle const& c)
{
    int n = p.size();

    // 1. Edge–circle intersections
    for (int i = 0; i < n; ++i) {
        shape::Line e { p[i], p[(i + 1) % n] };
        if (segment_circle_intersection(e, c))
            return true;
    }

    // 2. Circle center inside polygon
    if (Shape::Polygon(p).contains_point(c.center))
        return true;

    // 3. Polygon vertex inside circle
    double r2 = c.radius * c.radius;
    for (auto const& v : p) {
        double dx = v.x - c.center.x;
        double dy = v.y - c.center.y;
        if (dx*dx + dy*dy <= r2)
            return true;
    }

    return false;
}

bool Shape::circle_circle_intersection(shape::Circle const& c1, shape::Circle const& c2)
{
    double dx = c1.center.x - c2.center.x;
    double dy = c1.center.y - c2.center.y;
    double r  = c1.radius + c2.radius;

    return dx*dx + dy*dy <= r*r;
}

geo::Shape Shape::expand(float amount) const
{
    return std::visit(overloaded {
        [&](shape::Line const&)         { throw std::runtime_error("Can't expand single lines"); return Line({0,0},{0,0}); },
        [&](shape::Polygon const&)      { throw std::runtime_error("Sorry, not implemented yet.");  return Line({0,0},{0,0}); /* TODO */ },
        [&](shape::Circle const& c)     { return Shape::Circle(c.center, c.radius + amount); },
        [&](shape::Capsule const& c)    { return Shape::Capsule(c.p1, c.p2, c.radius + amount); }
    }, for_visit());
}

namespace shape {

Shape Capsule::polygon() const
{
    return Shape::ThickLine(p1, p2, radius);
}

std::array<Shape, 3> Capsule::subshapes() const
{
    return {
        Shape::Circle(p1, radius),
        Shape::Circle(p2, radius),
        Shape::ThickLine(p1, p2, radius)
    };
}

std::vector<Line> polygon_lines(Polygon const& poly)
{
    std::vector<Line> lines;
    lines.reserve(poly.size());
    for (size_t i = 0; i < poly.size() - 1; ++i)
        lines.emplace_back(poly.at(i), poly.at(i + 1));
    lines.emplace_back(poly.at(poly.size() - 1), poly.at(0));
    return lines;
}

}

}

size_t std::hash<geo::Shape>::operator()(const geo::Shape& shp) const noexcept
{
    auto hash_combine = [](size_t seed, size_t value, size_t position = 0) {
        const size_t prime = 1099511628211ULL * (position + 1);
        seed ^= value;
        seed *= prime;
        return seed;
    };

    return std::visit(overloaded {
        [&](geo::shape::Polygon const& poly) {
            size_t seed = 0, position = 0;
            for (auto const& p: poly)
                seed = hash_combine(seed, std::hash<geo::Point>()(p), position++);
            return seed;
        },
        [&](geo::shape::Circle const& c) {
            uint32_t float_bits;
            std::memcpy(&float_bits, &c.radius, sizeof(float_bits));
            return hash_combine(std::hash<geo::Point>()(c.center), float_bits);
        },
        [&](geo::shape::Line const& ln) {
            size_t a = std::hash<geo::Point>()(ln.p1);
            size_t b = std::hash<geo::Point>()(ln.p2);
            return hash_combine(a, b);
        },
        [&](geo::shape::Capsule const& c) {
            size_t seed = 0, position = 0;
            for (auto const& ss: c.subshapes())
                seed = hash_combine(seed, std::hash<geo::Shape>()(ss), position++);
            return seed;
        },
    }, shp.for_visit());

}
