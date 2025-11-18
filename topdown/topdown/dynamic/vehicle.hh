#ifndef TOPDOWN_VEHICLE_HH
#define TOPDOWN_VEHICLE_HH

#include <cassert>

#include "../vehicleconfig.hh"
#include "../world.hh"
#include "dymamicobject.hh"
#include "wheel.hh"

namespace topdown {

class Vehicle : public DynamicObject {
public:
    Vehicle(class World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg);

    void set_accelerator(bool accelerator);
    void set_reverse(bool reverse);
    void set_steering(float steering) { steering_ = steering; }

    void step() override;
    void attach(Vehicle* load);

    void shapes(std::vector<geo::Shape>& shp) const override;
    void touch_sensor(Sensor* sensor) override;
    void untouch_sensor(Sensor* sensor) override;

    [[nodiscard]] VehicleConfig const& config() const { return cfg_; }

    static SensorModifier default_modifier;

private:
    [[maybe_unused]] VehicleConfig const& cfg_;
    SensorModifier                      mod_ = default_modifier;
    std::vector<std::unique_ptr<Wheel>> front_wheels_ {}, rear_wheels_ {};
    std::vector<b2JointId>              front_joints_ {};
    float                               steering_ = 0.f;
    bool                                reverse_ = false;

    b2BodyId build_body(topdown::World const &world, b2Vec2 initial_pos, VehicleConfig const& config);
    void update_modifiers();
};

}

#endif //TOPDOWN_VEHICLE_HH
