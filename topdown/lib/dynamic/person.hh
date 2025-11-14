#ifndef TOPDOWN_PERSON_HH
#define TOPDOWN_PERSON_HH

#include "dymamicobject.hh"

namespace topdown {

class Person : public DynamicObject {
public:
    Person(class World const& world, b2Vec2 initial_pos);

    void step() override;

    void set_move(float x, float y);

private:
    b2BodyId build_body(class World const& world, b2Vec2 initial_pos);
    float move_x_ = 0.f, move_y_ = 0.f;
};

}

#endif //TOPDOWN_PERSON_HH
