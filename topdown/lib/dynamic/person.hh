#ifndef TOPDOWN_PERSON_HH
#define TOPDOWN_PERSON_HH

#include "dymamicobject.hh"

namespace topdown {

class Person : public DynamicObject {
public:
    Person(class World const& world, b2Vec2 initial_pos);

    void step() override;

private:
    b2BodyId build_body(class World const& world, b2Vec2 initial_pos);
};

}

#endif //TOPDOWN_PERSON_HH
