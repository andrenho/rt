#include "missile.hh"

#include "../world.hh"

namespace topdown {

Missile::Missile(World const& world, Object* originator, b2Vec2 target, float power)
    : DynamicObject(build_body(world, originator->center()))
{

}

b2BodyId Missile::build_body(World const& world, b2Vec2 initial_pos)
{
    // body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = initial_pos;
    body_def.motionLocks = { false, false, true };
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    // shape
    b2Circle circle = { .center = { 0, 0 }, .radius = 1.f };
    b2ShapeDef shape_def = default_shape();
    shape_def.density = 1.f;
    shape_def.material.friction = 0.3f;
    b2CreateCircleShape(body_id, &shape_def, &circle);

    return body_id;
}

}