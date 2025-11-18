#ifndef TOPDOWN_PUSHABLEOBJECT_HH
#define TOPDOWN_PUSHABLEOBJECT_HH

#include "dymamicobject.hh"
#include "topdown/world.hh"

namespace topdown {

class PushableObject : public DynamicObject {
public:
    PushableObject(class World const& world, geo::Shape const& shape, float mass);

    b2BodyId build_body(class World const& world, geo::Shape const& shape, float mass);
};

}

#endif //TOPDOWN_PUSHABLEOBJECT_HH
