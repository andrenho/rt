#ifndef TOPDOWN_WHEEL_HH
#define TOPDOWN_WHEEL_HH

#include <cassert>

#include "dymamicobject.hh"
#include "../vehicleconfig.hh"

namespace topdown {

class Wheel : public DynamicObject {
public:
    Wheel(class World const& world, VehicleConfig const& cfg);

    void step() override;

    void set_accelerator(bool accelerator) { accelerator_ = accelerator; }
    void set_breaks(bool breaks) { breaks_ = breaks; }

private:
    VehicleConfig const& cfg_;
    bool                 accelerator_ = false;
    bool                 breaks_ = false;

    static b2BodyId build_body(topdown::World const &world);
};

}

#endif //TOPDOWN_WHEEL_HH
