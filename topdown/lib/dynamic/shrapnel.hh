#ifndef TOPDOWN_SHRAPNEL_HH
#define TOPDOWN_SHRAPNEL_HH

#include "dymamicobject.hh"

namespace topdown {

class Shrapnel : public DynamicObject {
public:
    Shrapnel(class World const& world, b2Vec2 initial_pos, float angle, ExplosiveDef const& def);

private:
    b2BodyId build_body(class World const& world, b2Vec2 initial_pos, float angle, ExplosiveDef const& def);
};

}

#endif //TOPDOWN_SHRAPNEL_HH
