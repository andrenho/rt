#include "shrapnel.hh"

#include "../world.hh"

namespace topdown {

Shrapnel::Shrapnel(World const& world, b2Vec2 initial_pos, float angle, ExplosiveDef const& def)
        : DynamicObject(build_body(world, initial_pos, angle, def))
{

}

b2BodyId Shrapnel::build_body(World const& world, b2Vec2 initial_pos, float angle, ExplosiveDef const& def)
{
    const b2Vec2 ray_dir = {sin(angle), cos(angle) };

    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = initial_pos;
    body_def.linearVelocity = b2MulSV(def.blast_power, ray_dir);
    body_def.motionLocks = { false, false, true };
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    b2Circle circle({ 0, 0 }, .2f); // tiny

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1.f;
    shape_def.userData = this;
    b2CreateCircleShape(body_id, &shape_def, &circle);

    return body_id;
}

}