#include "explosive.hh"

#include "../world.hh"

namespace topdown {

Explosive::Explosive(World const& world, Object* originator, ExplosiveDef const& explosive_def)
        : DynamicObject(build_body(world, originator->center())), explosive_def_(explosive_def), originator_(originator)
{
}

b2BodyId Explosive::build_body(World const& world, b2Vec2 initial_pos)
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
    shape_def.density = 0.1f;
    shape_def.material.friction = 0.3f;
    shape_def.isSensor = true;
    shape_def.enableSensorEvents = true;
    b2CreateCircleShape(body_id, &shape_def, &circle);

    return body_id;
}

void Explosive::explode()
{
    printf("Boom!!!!\n");
}

}