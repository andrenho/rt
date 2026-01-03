#ifndef POINT_HH
#define POINT_HH

#include <cmath>
#include <random>
#include <vector>

namespace geo {

struct Point {
    template <typename T, typename U>
    Point(T x_, U y_) : x((float) x_), y((float) y_) {}

    explicit operator struct Size() const;

    Point operator+(Point const& a) const { return { x + a.x, y + a.y }; }
    Point operator-(Point const& a) const { return { x - a.x, y - a.y }; }
    Point operator*(float w) const { return { x * w, y * w }; }

    float x, y;

    static std::vector<geo::Point> grid(struct Bounds const& bounds, float avg_point_distance_w, float avg_point_distance_h);
    static std::vector<geo::Point> grid(class Shape const& area, float avg_point_distance_w, float avg_point_distance_h);
    static std::vector<geo::Point> grid(struct Bounds const& area, float avg_point_distance_w, float avg_point_distance_h, std::mt19937& rng, float randomness);
    static std::vector<geo::Point> grid(class Shape const& area, float avg_point_distance_w, float avg_point_distance_h, std::mt19937& rng, float randomness);

    static std::vector<geo::Point> relax_grid(std::vector<geo::Point> const& grid);

    static std::vector<geo::Point> poisson(struct Bounds const& bounds, float radius, uint64_t seed, uint32_t max_attemps=30);
    static std::vector<geo::Point> closest_points(std::vector<geo::Point> const& points, geo::Point const& center);

    bool operator==(const Point& other) const;
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


#endif //POINT_HH
