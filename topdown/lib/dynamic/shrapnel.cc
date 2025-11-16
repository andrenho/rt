#include "shrapnel.hh"

#include "../world.hh"

namespace topdown {

Shrapnel::Shrapnel(World const& world, b2Vec2 initial_pos, float angle, ExplosiveDef const& def)
        : DynamicObject(build_body(world, initial_pos, angle, def)), initial_pos_(b2Body_GetWorldCenterOfMass(id_)),
          max_distance_sq_(def.radius_sq * def.radius_sq)
{

}

b2BodyId Shrapnel::build_body(World const& world, b2Vec2 initial_pos, float angle, ExplosiveDef const& def)
{
    const b2Vec2 ray_dir = {sin(angle), cos(angle) };

    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = initial_pos;
    body_def.linearVelocity = b2MulSV(def.speed * 5.f, ray_dir);
    body_def.motionLocks = { false, false, true };
    body_def.isBullet = true;
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    b2Circle circle({ 0, 0 }, .2f); // tiny

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 50.f;
    shape_def.userData = this;
    b2CreateCircleShape(body_id, &shape_def, &circle);

    return body_id;
}

void Shrapnel::step()
{
    DynamicObject::step();

    float distance_sq = b2LengthSquared(b2Body_GetWorldCenterOfMass(id_) - initial_pos_);
    if (distance_sq >= max_distance_sq_)
        schedule_myself_for_deletion();
}

}