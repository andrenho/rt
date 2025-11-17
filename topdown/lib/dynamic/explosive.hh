#ifndef TOPDOWN_EXPLOSIVE_HH
#define TOPDOWN_EXPLOSIVE_HH

#include "dymamicobject.hh"

namespace topdown {

class Explosive : public DynamicObject {
public:
    Explosive(class World const& world, Object* originator, ExplosiveDef const& explosive_def);

    [[nodiscard]] bool explodes_on_contact() const override { return true; }
    [[nodiscard]] Object* originator() const { return originator_; }

    void explode();

protected:
    ExplosiveDef explosive_def_;

private:
    Object* originator_;

    b2BodyId build_body(class World const& world, b2Vec2 initial_pos);
};

}

#endif //TOPDOWN_EXPLOSIVE_HH
