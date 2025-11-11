#ifndef TOPDOWN_SHAPES_HH
#define TOPDOWN_SHAPES_HH

#include "box2d/box2d.h"

#include <utility>
#include <variant>
#include <vector>

namespace topdown {

using Line = std::pair<b2Vec2, b2Vec2>;

using Polygon = std::vector<b2Vec2>;

struct Circle {
    b2Vec2 center;
    float  radius;
};

struct Shape {
    std::variant<Polygon, Circle, Line> shape;
    uint8_t color = 0;
};

}

#endif //TOPDOWN_SHAPES_HH
