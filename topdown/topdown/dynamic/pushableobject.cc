#include "pushableobject.hh"

#include "../world.hh"

topdown::PushableObject::PushableObject(World const& world, geo::Shape const& shape, float mass)
        : DynamicObject(build_body(world, shape, mass))
{
}

b2BodyId topdown::PushableObject::build_body(topdown::World const& world, geo::Shape const& shape, float mass)
{
    // body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = { 0, 0 };
    b2BodyId body_id = b2CreateBody(world.id(), &body_def);

    // shape
    create_b2shape(body_id, shape, false, this);

    // set mass
    b2MassData mass_data = b2Body_GetMassData(body_id);
    mass_data.mass = mass;
    b2Body_SetMassData(body_id, mass_data);

    b2Body_SetAngularDamping(body_id, 3.f);
    b2Body_SetLinearDamping(body_id, 1.f);

    return body_id;
}
