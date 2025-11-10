#ifndef TOPDOWN_WORLD_HH
#define TOPDOWN_WORLD_HH

#include "box2d/box2d.h"

#include <concepts>
#include <memory>
#include <vector>

#include "vehicle.hh"

namespace topdown {

class World {
public:
    World();
    ~World();

    void step();

    [[nodiscard]] b2WorldId const& id() const { return id_; }

    template <typename T, typename ...Params> requires std::derived_from<T, Object>
    T* add_object(Params&&... params) {
        objects_.emplace_back(std::make_unique<T>(*this, std::forward<Params>(params)...));
        return (T*) std::prev(objects_.end())->get();
    }

    [[nodiscard]] std::vector<std::unique_ptr<Object>> const& objects() const { return objects_; }

private:
    b2WorldId id_ {};
    std::vector<std::unique_ptr<Object>> objects_;
};

}

#endif //TOPDOWN_WORLD_HH
