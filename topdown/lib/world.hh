#ifndef TOPDOWN_WORLD_HH
#define TOPDOWN_WORLD_HH

#include "box2d/box2d.h"

#include <concepts>
#include <memory>
#include <vector>

#include "dynamic/vehicle.hh"
#include "dynamic/wheel.hh"

namespace topdown {

class World {
public:
    World();
    ~World();

    World(World const&) = delete;
    World& operator=(World const&) = delete;

    void step();

    template <typename T, typename ...Params> requires std::derived_from<T, DynamicObject>
    T* add_object(Params&&... params) {
        dynamic_objects_.emplace_back(std::make_unique<T>(*this, std::forward<Params>(params)...));
        return (T*) std::prev(dynamic_objects_.end())->get();
    }

    [[nodiscard]] b2WorldId const& id() const { return id_; }
    [[nodiscard]] std::vector<std::unique_ptr<DynamicObject>> const& dynamic_objects() const { return dynamic_objects_; }

private:
    b2WorldId id_ {};
    std::vector<std::unique_ptr<DynamicObject>> dynamic_objects_ {};
};

}

#endif //TOPDOWN_WORLD_HH
