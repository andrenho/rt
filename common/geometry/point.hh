#ifndef POINT_HH
#define POINT_HH

#include <cmath>

namespace geo {

struct Point {
    Point(float x_, float y_) : x(x_), y(y_) {}
    explicit operator struct Size() const;

    Point operator+(Point const& a) const { return { x + a.x, y + a.y }; }

    float x, y;
};

struct Size {
    Size() : w(0), h(0) {}
    Size(float w_, float h_) : w(w_), h(h_) {}
    explicit operator Point() const { return { w, h }; };

    float w, h;
};

inline Point::operator Size() const { return { x, y }; }

struct Bounds {
    Bounds(Point top_left_, Point bottom_right_) : top_left(top_left_), bottom_right(bottom_right_) {}

    Point top_left;
    Point bottom_right;
};


// HASH

constexpr float POINT_PRECISION = 1e-4f;
inline float quantize(float v) {
    return std::round(v / POINT_PRECISION) * POINT_PRECISION;
}

inline bool operator==(Point const& a, Point const& b) {
    return quantize(a.x) == quantize(b.x)
            && quantize(a.y) == quantize(b.y);
}

struct PointHash {
    std::size_t operator()(Point const& p) const noexcept {
        float qx = quantize(p.x);
        float qy = quantize(p.y);

        std::size_t h1 = std::hash<float>{}(qx);
        std::size_t h2 = std::hash<float>{}(qy);

        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
    }
};

}

#endif //POINT_HH
