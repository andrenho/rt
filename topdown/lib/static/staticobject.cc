#include "staticobject.hh"

#include "../world.hh"

namespace topdown {

StaticObject::StaticObject(World const& world, std::vector<Shape> const& shapes, bool sensor)
    : shapes_(shapes)
{
    for (auto const& shp: shapes)
        shape_ids_.push_back(create_b2shape(world.static_body(), shp, sensor));
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

}