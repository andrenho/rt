#ifndef TOPDOWN_VEHICLE_HH
#define TOPDOWN_VEHICLE_HH

#include "vehicleconfig.hh"
#include "object.hh"
#include "world.hh"

namespace topdown {

class Vehicle : public Object {
public:
    Vehicle(class World const& world, b2Vec2 initial_pos, VehicleConfig const& cfg);
    ~Vehicle();

private:
    VehicleConfig const& cfg_;
    b2ShapeId shape_id_;

    b2BodyId build_body(topdown::World const &world, b2Vec2 initial_pos, VehicleConfig const &config);
};

}

#endif //TOPDOWN_VEHICLE_HH
