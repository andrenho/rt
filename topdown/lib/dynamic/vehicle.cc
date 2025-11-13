#include "vehicle.hh"

#include <numbers>
#include <stdexcept>

#include "../world.hh"

namespace topdown {

SensorModifier Vehicle::default_modifier = {
    .acceleration = .7f,
    .skid = .5f,
};

Vehicle::Vehicle(World const &world, b2Vec2 initial_pos, VehicleConfig const &cfg)
    : DynamicObject(build_body(world, initial_pos, cfg)), cfg_(cfg)
{
    // create wheels
    if (cfg.n_wheels == 4) {
        front_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg, initial_pos));
        front_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg, initial_pos));
        rear_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg, initial_pos));
        rear_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg, initial_pos));
    } else if (cfg.n_wheels == 2) {
        front_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg, initial_pos));
        rear_wheels_.emplace_back(std::make_unique<Wheel>(world, cfg, initial_pos));
    } else {
        throw std::runtime_error("Unsupported number of wheels.");
    }

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

    update_modifiers();
}

void Vehicle::step()
{
    for (auto joint: front_joints_)
        b2RevoluteJoint_SetLimits(joint, steering_ * .2f * cfg_.steering, steering_ * .2f * cfg_.steering);

    for (auto const& wheel: front_wheels_)
        wheel->step();
    for (auto const& wheel: rear_wheels_)
        wheel->step();
}

void Vehicle::attach(Vehicle* load)
{
    b2RevoluteJointDef joint_def = b2DefaultRevoluteJointDef();
    joint_def.base.bodyIdA = id_;
    joint_def.base.bodyIdB = load->id();
    joint_def.base.collideConnected = false;
    joint_def.enableLimit = true;
    joint_def.lowerAngle = -2.f;
    joint_def.upperAngle = 2.f;
    joint_def.base.localFrameA = { { 0, -(cfg_.h) }, b2Rot_identity };
    joint_def.base.localFrameB = { { 0, load->config().h }, b2Rot_identity };
    b2CreateRevoluteJoint(b2Body_GetWorld(id_), &joint_def);
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
    shape_def.enableSensorEvents = true;
    shape_def.density = 1.0f;
    shape_def.material.friction = 0.3f;
    shape_def.userData = (void *) this;
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

void Vehicle::touch_sensor(Sensor* sensor)
{
    DynamicObject::touch_sensor(sensor);
    update_modifiers();
}

void Vehicle::untouch_sensor(Sensor* sensor)
{
    DynamicObject::untouch_sensor(sensor);
    update_modifiers();
}

void Vehicle::update_modifiers()
{
    mod_ = default_modifier;
    for (auto const& sensor: touching_sensor_) {
        auto omod = sensor->sensor_modifier();
        if (omod)
            mod_ = *omod;
    }

    for (auto const& wheel: front_wheels_)
        wheel->set_modifier(mod_);
    for (auto const& wheel: rear_wheels_)
        wheel->set_modifier(mod_);
}

}