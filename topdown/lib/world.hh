#ifndef TOPDOWN_WORLD_HH
#define TOPDOWN_WORLD_HH

#include "box2d/box2d.h"

namespace topdown {

class World {
public:
    World();
    ~World();

private:
    b2WorldId id_ {};
};

}

#endif //TOPDOWN_WORLD_HH
