#include "world.hh"

namespace topdown {

World::World()
{
    // world
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = b2Vec2 {0.0f, 0.0f};
    world_def.userData = this;
    id_ = b2CreateWorld(&world_def);

    // static body
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_staticBody;
    static_body_ = b2CreateBody(id_, &body_def);
}

World::~World()
{
    dynamic_objects_.clear();
    static_objects_.clear();
    b2DestroyBody(static_body_);
    b2DestroyWorld(id_);
}

std::vector<Event> World::step()
{
    for (auto& obj: dynamic_objects_)
        obj->step();  // TODO - make this faster
    b2World_Step(id_, 1.0f / 60.0f, 4);

    std::vector<Event> events;
    add_sensor_events(events);
    add_hit_events(events);
    return events;
}

void World::add_sensor_events(std::vector<Event>& events) const
{
    b2SensorEvents sensor_events = b2World_GetSensorEvents(id_);

    for (int i = 0; i < sensor_events.beginCount; ++i) {
        b2SensorBeginTouchEvent* touch = sensor_events.beginEvents + i;
        if (b2Shape_IsValid(touch->sensorShapeId) && b2Shape_IsValid(touch->visitorShapeId)) {
            Object* sensor = (Sensor *) b2Shape_GetUserData(touch->sensorShapeId);
            DynamicObject* dynamic_object = (DynamicObject *) b2Shape_GetUserData(touch->visitorShapeId);
            if (sensor->is_sensor()) {
                dynamic_object->touch_sensor((Sensor *) sensor);
                events.emplace_back(BeginSensorEvent { (Sensor *) sensor, dynamic_object });
            } else if (sensor->is_explosive() && dynamic_object != ((Explosive *) sensor)->originator()) {
                ((Explosive *) sensor)->explode();
            }
        }
    }

    for (int i = 0; i < sensor_events.endCount; ++i) {
        b2SensorEndTouchEvent* touch = sensor_events.endEvents + i;
        if (b2Shape_IsValid(touch->sensorShapeId) && b2Shape_IsValid(touch->visitorShapeId)) {
            Sensor* sensor = (Sensor *) b2Shape_GetUserData(touch->sensorShapeId);
            DynamicObject* dynamic_object = (DynamicObject *) b2Shape_GetUserData(touch->visitorShapeId);
            if (sensor->is_sensor()) {
                dynamic_object->untouch_sensor(sensor);
                events.emplace_back(EndSensorEvent { sensor, dynamic_object });
            }
        }
    }

}

void World::add_hit_events(std::vector<Event>& events) const
{
    b2ContactEvents contactEvents = b2World_GetContactEvents(id_);

    for (int i = 0; i < contactEvents.hitCount; ++i) {
        b2ContactHitEvent* hit_event = contactEvents.hitEvents + i;
        if (b2Shape_IsValid(hit_event->shapeIdA) && b2Shape_IsValid(hit_event->shapeIdB)) {
            Object* object_1 = (Object *) b2Shape_GetUserData(hit_event->shapeIdA);
            Object* object_2 = (Object *) b2Shape_GetUserData(hit_event->shapeIdB);
            events.emplace_back(HitEvent { object_1, object_2, hit_event->approachSpeed });
        }
    }
}

}