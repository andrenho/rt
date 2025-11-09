#ifndef TOPDOWN_VEHICLE_HH
#define TOPDOWN_VEHICLE_HH

#include "vehicleconfig.hh"
#include "object.hh"

namespace topdown {

class Vehicle : public Object {
public:
    Vehicle(class World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg);
    ~Vehicle();

private:
    b2BodyId id_;
};

}

#endif //TOPDOWN_VEHICLE_HH
