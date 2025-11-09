#include "vehicle.hh"

#include "world.hh"

namespace topdown {

Vehicle::Vehicle(World const &world, b2Vec2 initial_pos, VehicleConfig const &cfg)
    : Object(build_body(world, initial_pos, cfg)), cfg_(cfg)
{
}

b2BodyId Vehicle::build_body(World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.position = initial_pos;
    b2BodyId id = b2CreateBody(world.id(), &body_def);

    b2Polygon box = b2MakeBox(cfg.w, cfg.h);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1.0f;
    shape_def.material.friction = 0.3f;
    shape_id_ = b2CreatePolygonShape(id, &shape_def, &box);

    return id;
}

Vehicle::~Vehicle()
{
    b2DestroyBody(id_);
}

}