#ifndef TOPDOWN_SHAPES_HH
#define TOPDOWN_SHAPES_HH

#include "box2d/box2d.h"

#include "geometry/shapes.hh"

#include <utility>
#include <variant>
#include <vector>

namespace topdown {

b2Vec2     b2vec(geo::Point const& p);
geo::Point point(b2Vec2 const& v);
b2ShapeId create_b2shape(b2BodyId body_id, geo::Shape const& shape, bool sensor=false, void* userdata=nullptr);

}

#endif //TOPDOWN_SHAPES_HH
