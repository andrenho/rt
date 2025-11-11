#include "wheel.hh"

#include <numbers>

#include "../world.hh"

namespace topdown {

Wheel::Wheel(World const& world, VehicleConfig const& cfg)
    : Object(build_body(world)), cfg_(cfg)
{
}

void Wheel::step()
{
    b2Vec2 center = b2Body_GetWorldCenterOfMass(id_);
    b2Vec2 current_normal = b2Body_GetWorldVector(id_, { 0, 1 });
    b2Vec2 f_vel = forward_velocity();

    // acceleration
    if (accelerator_) {
        b2Vec2 force = cfg_.acceleration * current_normal * 100.f;
        b2Body_ApplyForce(id_, force, center, true);
    }

    // drag
    float forward_speed = b2Length(b2Normalize(f_vel));
    b2Body_ApplyForce(id_, -cfg_.acceleration * forward_speed * .5f * f_vel, center, true);

    // breaks
    if (breaks_)
        b2Body_ApplyForce(id_, -cfg_.acceleration * forward_speed * f_vel * 1.f, center, true);

    // kill lateral velocity
    b2Vec2 impulse = b2Body_GetMass(id_) * 1.5f * -lateral_velocity();
    b2Body_ApplyLinearImpulse(id_, impulse, b2Body_GetWorldCenterOfMass(id_), true);
}

b2BodyId Wheel::build_body(World const& world)
{
    // body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = { 0, 0 };
    body_def.rotation = b2MakeRot((float) std::numbers::pi);
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    // shape
    b2Polygon box = b2MakeBox(.2f, .6f);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1.0f;
    shape_def.material.friction = 0.3f;
    b2CreatePolygonShape(body_id, &shape_def, &box);

    return body_id;
}

}