#ifndef POINT_HH
#define POINT_HH

namespace geo {

struct Point {
    Point(float x, float y) : x(x), y(y) {}
    explicit operator struct Size() const;

    Point operator+(Point const& a) const { return { x + a.x, y + a.y }; }

    const float x, y;
};

struct Size {
    Size(float w, float h) : w(w), h(h) {}
    explicit operator Point() const { return { w, h }; };

    const float w, h;
};

inline Point::operator Size() const { return { x, y }; }

struct Bounds {
    Bounds(Point top_left, Point bottom_right) : top_left(top_left), bottom_right(bottom_right) {}

    Point top_left;
    Point bottom_right;
};

}

#endif //POINT_HH
