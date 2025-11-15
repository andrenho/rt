#ifndef TOPDOWN_EVENT_HH
#define TOPDOWN_EVENT_HH

#include <variant>

#include "dynamic/dymamicobject.hh"
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

struct HitEvent {
    Object* object1;
    Object* object2;
    float   approach_speed;
};

using Event = std::variant<BeginSensorEvent, EndSensorEvent, HitEvent>;

}

#endif //TOPDOWN_EVENT_HH
