#include "world.hh"

namespace topdown {

World::World()
{
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = (b2Vec2){0.0f, 0.0f};
    id_ = b2CreateWorld(&world_def);
}

World::~World()
{
    b2DestroyWorld(id_);
}

}