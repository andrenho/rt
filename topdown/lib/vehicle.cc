#include "vehicle.hh"

#include "world.hh"

namespace topdown {

Vehicle::Vehicle(World const &world, b2Vec2 initial_pos, VehicleConfig const &cfg)
    : Object(build_body(world, initial_pos, cfg)), cfg_(cfg)
{
}

b2BodyId Vehicle::build_body(World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg)
{
    // body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = initial_pos;
    body_def.rotation = b2MakeRot(M_PI);
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    // shape
    b2Polygon box = b2MakeBox(cfg.w, cfg.h);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1.0f;
    shape_def.material.friction = 0.3f;
    shape_id_ = b2CreatePolygonShape(body_id, &shape_def, &box);

    return body_id;
}

Vehicle::~Vehicle()
{
    b2DestroyBody(id_);
}

void Vehicle::step()
{
    b2Vec2 current_normal = b2Body_GetWorldVector(id_, { 0, 1 });
    b2Vec2 f_vel = forward_velocity();

    // acceleration
    if (accelerator_)
        b2Body_ApplyForce(id_, cfg_.acceleration * current_normal, b2Body_GetWorldCenterOfMass(id_), true);

    // drag
    float forward_speed = b2Length(b2Normalize(f_vel));
    b2Body_ApplyForce(id_, -2 * forward_speed * f_vel, b2Body_GetWorldCenterOfMass(id_), true);

    /*
    if (accelerator_) {
        float forward_speed = b2Length(b2Normalize(f_vel));
        b2Body_ApplyForce(id_, -2 * forward_speed * f_vel, b2Body_GetWorldCenterOfMass(id_), true);
    }
    */

    /*
    if (accelerator_) {
        b2Vec2 impulse = b2Body_GetMass(id_) * -lateral_velocity();
        b2Body_ApplyLinearImpulse(id_, impulse, b2Body_GetWorldCenterOfMass(id_), true);
    }
     */
}

}