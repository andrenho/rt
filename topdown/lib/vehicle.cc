#include "vehicle.hh"

#include "world.hh"

namespace topdown {

Vehicle::Vehicle(World const &world, b2Vec2 initial_pos, VehicleConfig const &cfg)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.position = initial_pos;
    id_ = b2CreateBody(world.id(), &body_def);
}

Vehicle::~Vehicle()
{

}

}