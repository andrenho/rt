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
    body_def.rotation = b2MakeRot((float) M_PI);
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    // shape
    b2Polygon box = b2MakeBox(cfg.w, cfg.h);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1.0f;
    shape_def.material.friction = 0.3f;
    b2CreatePolygonShape(body_id, &shape_def, &box);

    return body_id;
}

Vehicle::~Vehicle()
{
    b2DestroyBody(id_);
}

b2Vec2 Vehicle::front_wheel_vec() const
{
    return b2Body_GetWorldPoint(id_, b2Vec2 { 0, cfg_.wheelbase });
}

b2Vec2 Vehicle::rear_wheel_vec() const
{
    return b2Body_GetWorldPoint(id_, b2Vec2 { 0, -cfg_.wheelbase });
}

std::vector<Vehicle::Force> Vehicle::iteration() const
{
    std::vector<Force> forces;

    b2Vec2 current_normal = b2Body_GetWorldVector(id_, { 0, 1 });
    b2Vec2 f_vel = forward_velocity();

    // acceleration
    if (accelerator_) {
        b2Vec2 force = cfg_.acceleration * current_normal * 100.f;
        force = b2RotateVector(b2MakeRot(steering_ * (float) M_PI_4), force);
        forces.emplace_back(front_wheel_vec(), force, 1);
    }

    // drag
    float forward_speed = b2Length(b2Normalize(f_vel));
    forces.emplace_back(b2Body_GetWorldCenterOfMass(id_), -cfg_.acceleration * forward_speed * f_vel, 2);

    // breaks
    if (breaks_) {
        forces.emplace_back(front_wheel_vec(), -cfg_.acceleration * forward_speed * f_vel * 10.f, 3);
        forces.emplace_back(rear_wheel_vec(), -cfg_.acceleration * forward_speed * f_vel * 10.f, 3);
    }

    return forces;
}

}