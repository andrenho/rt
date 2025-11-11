#include "world.hh"
#include "staticobject.hh"

namespace topdown {

World::World()
{
    // world
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = b2Vec2 {0.0f, 0.0f};
    id_ = b2CreateWorld(&world_def);

    static_objects_ = add_object<StaticObjects>();
}

World::~World()
{
    objects_.clear();
    b2DestroyWorld(id_);
}

void World::step()
{
    for (auto& obj: objects_)
        obj->step();  // TODO - make this faster
    b2World_Step(id_, 1.0f / 60.0f, 4);
}

void World::add_static_shape(Shape const& shape)
{
    static_objects_->add_shape(shape);
}

}