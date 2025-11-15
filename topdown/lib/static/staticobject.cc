#include "staticobject.hh"

#include "../world.hh"

namespace topdown {

StaticObject::StaticObject(World const& world, std::vector<Shape> const& shapes, bool sensor)
    : shapes_(shapes)
{
    for (auto const& shp: shapes)
        shape_ids_.push_back(create_b2shape(world.static_body(), shp, sensor, this));

    // calculate center
    b2Vec2 lower_bound = { FLT_MAX, FLT_MAX }, upper_bound = { FLT_MIN, FLT_MIN };
    for (b2ShapeId shape_id: shape_ids_) {
        b2AABB aabb = b2Shape_GetAABB(shape_id);
        lower_bound.x = std::min(lower_bound.x, aabb.lowerBound.x);
        lower_bound.y = std::min(lower_bound.x, aabb.lowerBound.y);
        upper_bound.x = std::max(upper_bound.x, aabb.upperBound.x);
        upper_bound.y = std::max(upper_bound.y, aabb.upperBound.y);
    }
    center_ = b2AABB_Center(b2AABB { .lowerBound = lower_bound, .upperBound = upper_bound });
}

StaticObject::~StaticObject()
{
    for (auto id: shape_ids_)
        b2DestroyShape(id, false);
}

void StaticObject::shapes(std::vector<Shape>& shp) const
{
    for (auto const& s: shapes_)
        shp.push_back(s);
}

b2WorldId StaticObject::world_id() const
{
    return b2Shape_GetWorld(shape_ids_.at(0));
}

b2Vec2 StaticObject::center() const
{
    return center_;
}

}