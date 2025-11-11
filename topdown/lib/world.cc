#include "world.hh"

namespace topdown {

World::World()
{
    // world
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = b2Vec2 {0.0f, 0.0f};
    id_ = b2CreateWorld(&world_def);

    // static body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_staticBody;
    static_body_ = b2CreateBody(id_, &body_def);
}

World::~World()
{
    dynamic_objects_.clear();
    b2DestroyBody(static_body_);
    b2DestroyWorld(id_);
}

void World::step()
{
    for (auto& obj: dynamic_objects_)
        obj->step();  // TODO - make this faster
    b2World_Step(id_, 1.0f / 60.0f, 4);
}

}