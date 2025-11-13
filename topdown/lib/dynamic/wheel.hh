#ifndef TOPDOWN_WHEEL_HH
#define TOPDOWN_WHEEL_HH

#include <cassert>

#include "dymamicobject.hh"
#include "../vehicleconfig.hh"

namespace topdown {

class Wheel : public DynamicObject {
public:
    Wheel(class World const& world, VehicleConfig const& cfg, b2Vec2 initial_pos);

    void step() override;

    void set_accelerator(bool accelerator) { accelerator_ = accelerator; }
    void set_reverse(bool reverse) { reverse_ = reverse; }
    void set_modifier(SensorModifier const& mod) { mod_ = mod; }

private:
    VehicleConfig const& cfg_;
    SensorModifier       mod_;
    bool                 accelerator_ = false;
    bool                 reverse_ = false;

    static b2BodyId build_body(topdown::World const &world, b2Vec2 initial_pos);
};

}

#endif //TOPDOWN_WHEEL_HH
