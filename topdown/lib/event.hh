#ifndef TOPDOWN_EVENT_HH
#define TOPDOWN_EVENT_HH

#include <variant>
#include "object.hh"
#include "static/sensor.hh"

namespace topdown {

struct BeginSensorEvent {
    Sensor*        sensor;
    DynamicObject* object;
};

struct EndSensorEvent {
    Sensor*        sensor;
    DynamicObject* object;
};

using Event = std::variant<BeginSensorEvent, EndSensorEvent>;

}

#endif //TOPDOWN_EVENT_HH
