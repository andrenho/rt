#ifndef TOPDOWN_STATICOBJECT_HH
#define TOPDOWN_STATICOBJECT_HH

#include "object.hh"

namespace topdown {

class StaticObjects : public Object {
public:
    explicit StaticObjects(class World const& world);

private:
    static b2BodyId build_body(topdown::World const &world);
};

}

#endif //TOPDOWN_STATICOBJECT_HH
