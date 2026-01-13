#ifndef POINT_HH
#define POINT_HH

#include <cmath>
#include <vector>

#include "random/random.hh"

namespace geo {

struct Point {
    template <typename T, typename U>
    Point(T x_, U y_) : x((float) x_), y((float) y_) {}

    explicit operator struct Size() const;

    Point operator+(Point const& a) const { return { x + a.x, y + a.y }; }
    Point operator-(Point const& a) const { return { x - a.x, y - a.y }; }
    Point operator*(float w) const { return { x * w, y * w }; }

    float x, y;

    [[nodiscard]] float angle(Point const& other) const;
    [[nodiscard]] float dot(Point const& other) const;
    [[nodiscard]] float length_sq() const;

    static std::vector<Point> grid(struct Bounds const& bounds, float avg_point_distance_w, float avg_point_distance_h);
    static std::vector<Point> grid(class Shape const& area, float avg_point_distance_w, float avg_point_distance_h);
    static std::vector<Point> grid(struct Bounds const& area, float avg_point_distance_w, float avg_point_distance_h, Random& random, float randomness);
    static std::vector<Point> grid(class Shape const& area, float avg_point_distance_w, float avg_point_distance_h, Random& rng, float randomness);

    static std::vector<Point> relax_grid(std::vector<Point> const& grid);

    static std::vector<Point> poisson(class Shape const& bounds, float radius, uint64_t seed, uint32_t max_attemps=30);
    static std::vector<Point> closest_points(std::vector<Point> const& points, Point const& center, size_t n_points);

    static bool segment_intersection(Point const& p1, Point const& p2, Point const& q1, Point const& q2, Point* out);

    static Shape convex_hull(std::vector<Point> const& points);

    bool operator==(const Point& other) const;
};

struct UPoint {
    UPoint() : x(0), y(0) {}
    UPoint(size_t x_, size_t y_) : x(x_), y(y_) {}

    bool operator==(const UPoint& other) const { return x == other.x && y == other.y; }

    size_t x, y;
};

struct Size {
    Size() : w(0), h(0) {}
    Size(float w_, float h_) : w(w_), h(h_) {}
    explicit operator Point() const { return { w, h }; };

    float w, h;
};

struct Bounds {
    Bounds(Point top_left_, Point bottom_right_) : top_left(top_left_), bottom_right(bottom_right_) {}

    [[nodiscard]] bool intersects(Bounds const& a) const;

    Point top_left;
    Point bottom_right;
};

}

template<>
struct std::hash<geo::Point> {
    std::size_t operator()(const geo::Point& p) const noexcept {
        std::size_t h1 = std::hash<int>{}(static_cast<int>(std::round(p.x * 1000)));
        std::size_t h2 = std::hash<int>{}(static_cast<int>(std::round(p.y * 1000)));
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

template<>
struct std::hash<geo::UPoint> {
    std::size_t operator()(const geo::UPoint& p) const noexcept {
        std::size_t h1 = std::hash<int>{}(static_cast<int>(std::round(p.x * 1000)));
        std::size_t h2 = std::hash<int>{}(static_cast<int>(std::round(p.y * 1000)));
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

#endif //POINT_HH
