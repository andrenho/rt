#include "vehicle.hh"

#include <numbers>
#include <stdexcept>

#include "../world.hh"

namespace topdown {

Vehicle::Vehicle(World const &world, b2Vec2 initial_pos, VehicleConfig const &cfg)
    :DynamicObject(build_body(world, initial_pos, cfg)), cfg_(cfg)
{
    // front wheels (TODO - change according to car type)
    front_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg));
    front_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg));
    rear_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg));
    rear_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg));

    // create joints
    b2RevoluteJointDef joint_def = b2DefaultRevoluteJointDef();
    joint_def.base.bodyIdA = id_;
    joint_def.base.collideConnected = false;
    joint_def.enableLimit = true;
    joint_def.lowerAngle = 0;
    joint_def.upperAngle = 0;
    joint_def.base.localFrameB = { b2Vec2_zero, b2Rot_identity };

    bool right = false;
    for (auto const& wheel: front_wheels_) {
        joint_def.base.bodyIdB = wheel->id();
        joint_def.base.localFrameA = { { (right ? 1.f : -1.f) * (cfg.w - .2f), cfg.wheelbase }, b2Rot_identity };
        front_joints_.push_back(b2CreateRevoluteJoint(world.id(), &joint_def));
        right = true;
    }

    right = false;
    for (auto const& wheel: rear_wheels_) {
        joint_def.base.bodyIdB = wheel->id();
        joint_def.base.localFrameA = { { (right ? 1.f : -1.f) * (cfg.w - .2f), -cfg.wheelbase }, b2Rot_identity };
        b2CreateRevoluteJoint(world.id(), &joint_def);
        right = true;
    }
}

void Vehicle::step()
{
    for (auto joint: front_joints_)
        b2RevoluteJoint_SetLimits(joint, steering_ * .2f, steering_ * .2f);

    for (auto const& wheel: front_wheels_)
        wheel->step();
    for (auto const& wheel: rear_wheels_)
        wheel->step();
}

b2BodyId Vehicle::build_body(World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg)
{
    // body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = initial_pos;
    body_def.rotation = b2MakeRot((float) std::numbers::pi);
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    // shape
    b2Polygon box = b2MakeBox(cfg.w, cfg.h);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 1.0f;
    shape_def.material.friction = 0.3f;
    b2CreatePolygonShape(body_id, &shape_def, &box);

    return body_id;
}

void Vehicle::set_accelerator(bool accelerator)
{
    for (auto const& wheel: front_wheels_)
        wheel->set_accelerator(accelerator);
}

void Vehicle::set_breaks(bool breaks)
{
    for (auto const& wheel: rear_wheels_)
        wheel->set_breaks(breaks);
}

void Vehicle::shapes(std::vector<Shape>& shp) const
{
    DynamicObject::shapes(shp);
    for (auto const& wheel: front_wheels_)
        wheel->shapes(shp);
    for (auto const& wheel: rear_wheels_)
        wheel->shapes(shp);
}

}