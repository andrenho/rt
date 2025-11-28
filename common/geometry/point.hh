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

    static std::vector<geo::Point> grid(struct Bounds const& bounds, float avg_point_distance);
    static std::vector<geo::Point> grid(class Shape const& area, float avg_point_distance);
    static std::vector<geo::Point> grid(struct Bounds const& area, float avg_point_distance, std::mt19937& rng, float randomness);
    static std::vector<geo::Point> grid(class Shape const& area, float avg_point_distance, std::mt19937& rng, float randomness);
};


struct Size {
    Size() : w(0), h(0) {}
    Size(float w_, float h_) : w(w_), h(h_) {}
    explicit operator Point() const { return { w, h }; };

    float w, h;
};

struct Bounds {
    Bounds(Point top_left_, Point bottom_right_) : top_left(top_left_), bottom_right(bottom_right_) {}

    Point top_left;
    Point bottom_right;
};

}

#endif //POINT_HH
