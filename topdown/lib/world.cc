#include "world.hh"

#include <algorithm>
#include <stdexcept>

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
    remove_objects_scheduled_for_deletion();

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
            auto sensor = (Object *) b2Shape_GetUserData(touch->sensorShapeId);
            auto object = (Object *) b2Shape_GetUserData(touch->visitorShapeId);

            // check for explosive
            if (sensor->explodes_on_contact()) {
                if (object == ((Explosive *) sensor)->originator())
                    continue;
                auto explosive = dynamic_cast<Explosive *>(sensor);
                if (!explosive)
                    throw std::logic_error("Error: non-explosive exploding");
                explosive->explode();
                continue;
            }

            // check for sensor
            auto dynamic_object = dynamic_cast<DynamicObject *>(object);
            if (!dynamic_object)
                continue;
            auto psensor = dynamic_cast<Sensor *>(sensor);
            if (!psensor)
                throw std::logic_error("Error: explosive as sensor");
            dynamic_object->touch_sensor(psensor);
            events.emplace_back(BeginSensorEvent { psensor, dynamic_object });
        }
    }

    for (int i = 0; i < sensor_events.endCount; ++i) {
        b2SensorEndTouchEvent* touch = sensor_events.endEvents + i;
        if (b2Shape_IsValid(touch->sensorShapeId) && b2Shape_IsValid(touch->visitorShapeId)) {
            auto sensor = dynamic_cast<Sensor *>((Object *) b2Shape_GetUserData(touch->sensorShapeId));
            auto dynamic_object = dynamic_cast<DynamicObject *>((Object *) b2Shape_GetUserData(touch->visitorShapeId));
            if (!dynamic_object || !sensor)
                continue;
            dynamic_object->untouch_sensor(sensor);
            events.emplace_back(EndSensorEvent { sensor, dynamic_object });
        }
    }

}

void World::add_hit_events(std::vector<Event>& events) const
{
    b2ContactEvents contactEvents = b2World_GetContactEvents(id_);

    for (int i = 0; i < contactEvents.hitCount; ++i) {
        b2ContactHitEvent* hit_event = contactEvents.hitEvents + i;
        if (b2Shape_IsValid(hit_event->shapeIdA) && b2Shape_IsValid(hit_event->shapeIdB)) {
            auto object_1 = (Object *) b2Shape_GetUserData(hit_event->shapeIdA);
            auto object_2 = (Object *) b2Shape_GetUserData(hit_event->shapeIdB);
            auto shrapnel_1 = dynamic_cast<Shrapnel *>(object_1);
            auto shrapnel_2 = dynamic_cast<Shrapnel *>(object_2);
            if (shrapnel_1) {
                events.emplace_back(ExplosionEvent { object_2, hit_event->approachSpeed });
            } else if (shrapnel_2) {
                events.emplace_back(ExplosionEvent { object_1, hit_event->approachSpeed });
            } else {
                events.emplace_back(HitEvent { object_1, object_2, hit_event->approachSpeed });
            }
        }
    }
}

void World::remove_objects_scheduled_for_deletion()
{
    // fast removal algorithm

    for (auto obj: scheduled_for_deletion_) {
        {
            DynamicObject* dobj = dynamic_cast<DynamicObject *>(obj);
            if (dobj) {
                for (size_t i = 0; i < dynamic_objects_.size(); ++i) {
                    if (dynamic_objects_.at(i).get() == dobj) {
                        dynamic_objects_[i] = std::move(dynamic_objects_.back());
                        dynamic_objects_.pop_back();
                        goto next;
                    }
                }
            }
        }

        {
            StaticObject* sobj = dynamic_cast<StaticObject *>(obj);
            if (sobj) {
                for (size_t i = 0; i < static_objects_.size(); ++i) {
                    if (static_objects_.at(i).get() == sobj) {
                        static_objects_[i] = std::move(static_objects_.back());
                        static_objects_.pop_back();
                        goto next;
                    }
                }
            }
        }
next:
    }

    scheduled_for_deletion_.clear();
}

}