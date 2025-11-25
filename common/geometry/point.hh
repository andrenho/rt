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

}

#endif //POINT_HH
