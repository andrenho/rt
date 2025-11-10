#ifndef TOPDOWN_VEHICLE_HH
#define TOPDOWN_VEHICLE_HH

#include <cassert>

#include "vehicleconfig.hh"
#include "object.hh"
#include "world.hh"

namespace topdown {

class Vehicle : public Object {
public:
    Vehicle(class World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg);
    ~Vehicle() override;

    void set_accelerator(bool accelerator) { accelerator_ = accelerator; }
    void set_breaks(bool breaks) { breaks_ = breaks; }
    void set_steering(float steering) { assert(steering >= -1.f && steering <= 1.f); steering_ = steering; }

protected:
    [[nodiscard]] std::vector<Force> iteration() const override;

private:
    VehicleConfig const& cfg_;
    bool                 accelerator_ = false;
    bool                 breaks_ = false;
    float                steering_ = 0;

    [[nodiscard]] b2Vec2 front_wheel_vec() const;
    [[nodiscard]] b2Vec2 rear_wheel_vec() const;

    static b2BodyId build_body(topdown::World const &world, b2Vec2 initial_pos, VehicleConfig const &config);
};

}

#endif //TOPDOWN_VEHICLE_HH
