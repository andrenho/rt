#ifndef TOPDOWN_SENSOR_HH
#define TOPDOWN_SENSOR_HH

#include "staticobject.hh"

namespace topdown {

class Sensor : public StaticObject {
public:
    Sensor(class World const& world, std::vector<Shape> const& shapes);
    Sensor(class World const& world, Shape const& shape) : Sensor(world, std::vector<Shape> { shape }) {}
};

}

#endif //TOPDOWN_SENSOR_HH
