#ifndef TOPDOWN_MISSILE_HH
#define TOPDOWN_MISSILE_HH

#include "explosive.hh"

namespace topdown {

class Missile : public Explosive {
public:
    Missile(class World const& world, Object* originator, b2Vec2 target, ExplosiveDef const& explosive_def);

private:
    b2Vec2 target_;
};

}

#endif //TOPDOWN_MISSILE_HH
