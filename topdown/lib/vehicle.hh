#ifndef TOPDOWN_VEHICLE_HH
#define TOPDOWN_VEHICLE_HH

#include <cassert>

#include "vehicleconfig.hh"
#include "object.hh"
#include "world.hh"
#include "wheel.hh"

namespace topdown {

class Vehicle : public Object {
public:
    Vehicle(class World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg);

    void set_accelerator(bool accelerator);
    void set_breaks(bool breaks);
    void set_steering(float steering) { steering_ = steering; }

    void step() override;

    void shapes(std::vector<Shape>& shp) const override;

private:
    VehicleConfig const&                cfg_;
    std::vector<std::unique_ptr<Wheel>> front_wheels_ {}, rear_wheels_ {};
    std::vector<b2JointId>              front_joints_ {};
    float                               steering_;

    static b2BodyId build_body(topdown::World const &world, b2Vec2 initial_pos, VehicleConfig const &config);
};

}

#endif //TOPDOWN_VEHICLE_HH
