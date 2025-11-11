#include "staticobject.hh"

#include "world.hh"

namespace topdown {

StaticObjects::StaticObjects(World const& world)
    : Object(build_body(world))
{
}

b2BodyId StaticObjects::build_body(World const& world)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_staticBody;
    return b2CreateBody(world.id(), &body_def);
}

}