#ifndef TOPDOWN_MISSILE_HH
#define TOPDOWN_MISSILE_HH

#include "dymamicobject.hh"

namespace topdown {

class Missile : public DynamicObject {
public:
    Missile(class World const& world, Object* originator, b2Vec2 target, float power);

private:
    b2BodyId build_body(class World const& world, b2Vec2 initial_pos);
};

}

#endif //TOPDOWN_MISSILE_HH
