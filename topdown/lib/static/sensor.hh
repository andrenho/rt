#ifndef TOPDOWN_SENSOR_HH
#define TOPDOWN_SENSOR_HH

#include "staticobject.hh"

namespace topdown {

class Sensor : public StaticObject {
public:
    using StaticObject::StaticObject;

    Sensor(class World const& world, std::vector<Shape> const& shape);
};

}

#endif //TOPDOWN_SENSOR_HH
