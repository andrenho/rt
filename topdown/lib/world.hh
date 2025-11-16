#ifndef TOPDOWN_WORLD_HH
#define TOPDOWN_WORLD_HH

#include "box2d/box2d.h"

#include <concepts>
#include <memory>
#include <vector>

#include "dynamic/vehicle.hh"
#include "dynamic/wheel.hh"
#include "dynamic/person.hh"
#include "dynamic/pushableobject.hh"
#include "dynamic/explosive.hh"
#include "dynamic/missile.hh"
#include "static/staticobject.hh"
#include "static/sensor.hh"
#include "event.hh"

namespace topdown {

class World {
public:
    World();
    ~World();

    World(World const&) = delete;
    World& operator=(World const&) = delete;

    std::vector<Event> step();

    template <typename T, typename ...Params> requires std::derived_from<T, DynamicObject>
    T* add_object(Params&&... params) {
        auto obj = std::make_unique<T>(*this, std::forward<Params>(params)...);
        obj->setup();
        dynamic_objects_.emplace_back(std::move(obj));
        return (T*) std::prev(dynamic_objects_.end())->get();
    }

    template <typename T, typename ...Params> requires std::derived_from<T, StaticObject>
    T* add_object(Params&&... params) {
        auto obj = std::make_unique<T>(*this, std::forward<Params>(params)...);
        obj->setup();
        static_objects_.emplace_back(std::move(obj));
        return (T*) std::prev(static_objects_.end())->get();
    }

    void schedule_for_deletion(Object* object) { scheduled_for_deletion_.emplace_back(object); }

    [[nodiscard]] b2WorldId const& id() const { return id_; }
    [[nodiscard]] b2BodyId const& static_body() const { return static_body_; }
    [[nodiscard]] std::vector<std::unique_ptr<DynamicObject>> const& dynamic_objects() const { return dynamic_objects_; }
    [[nodiscard]] std::vector<std::unique_ptr<StaticObject>> const& static_objects() const { return static_objects_; }

private:
    b2WorldId id_ {};
    b2BodyId  static_body_ {};
    std::vector<std::unique_ptr<StaticObject>> static_objects_ {};
    std::vector<std::unique_ptr<DynamicObject>> dynamic_objects_ {};
    std::vector<Object*> scheduled_for_deletion_ {};

    void add_sensor_events(std::vector<Event>& event) const;
    void add_hit_events(std::vector<Event>& event) const;

    void remove_objects_scheduled_for_deletion();
};

}

#endif //TOPDOWN_WORLD_HH
