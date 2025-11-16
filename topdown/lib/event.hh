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
    float   force;
};

struct ExplosionEvent {
    Object* object;
    float   force;
};

using Event = std::variant<BeginSensorEvent, EndSensorEvent, HitEvent, ExplosionEvent>;

}

#endif //TOPDOWN_EVENT_HH
