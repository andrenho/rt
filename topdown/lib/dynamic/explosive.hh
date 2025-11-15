#ifndef TOPDOWN_EXPLOSIVE_HH
#define TOPDOWN_EXPLOSIVE_HH

#include "dymamicobject.hh"

namespace topdown {

class Explosive : public DynamicObject {
public:
    Explosive(class World const& world, Object* originator, float power);                 // landmine
    Explosive(class World const& world, Object* originator, b2Vec2 target, float power);  // missile

    [[nodiscard]] bool is_missile() const override { return true; }
    [[nodiscard]] Object* originator() const { return originator_; }

    void explode();

private:
    Object* originator_;
    float power_;

    b2BodyId build_body(class World const& world, b2Vec2 initial_pos);
};

}

#endif //TOPDOWN_EXPLOSIVE_HH
