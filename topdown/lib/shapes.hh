#ifndef TOPDOWN_SHAPES_HH
#define TOPDOWN_SHAPES_HH

#include "box2d/box2d.h"

#include <variant>
#include <vector>

namespace topdown {

using Polygon = std::vector<b2Vec2>;

struct Circle {
    b2Vec2 center;
    float  radius;
};

using Shape = std::variant<Polygon, Circle>;

}

#endif //TOPDOWN_SHAPES_HH
