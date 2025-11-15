#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include <limits>
#include <optional>

#include "shapes.hh"

namespace topdown {

struct Cast {
    struct Hit {
        class Object* object;
        b2Vec2        location;
        float         length;
    };
    std::optional<Hit> hit {};
    Object*            originator = nullptr;
    b2Vec2             final_point {};
};

class Object {
public:
    virtual ~Object() = default;
    virtual void shapes(std::vector<Shape>& shp) const = 0;

    Cast cast(b2Vec2 target, float max_distance=std::numeric_limits<float>::infinity());

    [[nodiscard]] virtual bool is_sensor() const { return false; }

    [[nodiscard]] virtual b2Vec2 get_center() const = 0;

protected:
    Object() = default;

    virtual b2WorldId get_world_id() const = 0;
};

}

#endif //TOPDOWN_OBJECT_HH
