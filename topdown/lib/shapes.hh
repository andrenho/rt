#ifndef TOPDOWN_SHAPES_HH
#define TOPDOWN_SHAPES_HH

#include "box2d/box2d.h"

#include <utility>
#include <variant>
#include <vector>

namespace topdown {

using Line = std::pair<b2Vec2, b2Vec2>;

using Polygon = std::vector<b2Vec2>;

inline Polygon Box(b2Vec2 p, b2Vec2 sz)
{
    return { { p.x, p.y }, { p.x, p.y + sz.y }, { p.x + sz.x, p.y + sz.y }, { p.x + sz.x, p.y } };
}

struct Circle {
    b2Vec2 center;
    float  radius;
};

using Shape = std::variant<Polygon, Circle, Line>;

b2ShapeId create_b2shape(b2BodyId body_id, Shape const& shape, bool sensor=false, void* userdata=nullptr);

}

#endif //TOPDOWN_SHAPES_HH
