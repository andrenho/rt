#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include <limits>
#include <optional>

#include "shapes.hh"
#include "explosivedef.hh"

namespace topdown {

struct Cast {
    struct Hit {
        const class Object* object;
        b2Vec2              location;
        float               length;
    };
    std::optional<Hit> hit {};
    const Object*      originator = nullptr;
    b2Vec2             final_point {};
};

class Object {
public:
    virtual ~Object() = default;
    virtual void shapes(std::vector<Shape>& shp) const = 0;

    [[nodiscard]] Cast cast(b2Vec2 target, float max_distance=std::numeric_limits<float>::infinity()) const;

    class Explosive* fire_missile(b2Vec2 target, float speed, ExplosiveDef const& explosive_def);
    class Explosive* place_explosive(ExplosiveDef const& explosive_def);

    [[nodiscard]] virtual bool is_sensor() const { return false; }
    [[nodiscard]] virtual bool is_explosive() const { return false; }
    [[nodiscard]] virtual b2Vec2 center() const = 0;

    [[nodiscard]] class World& world();
    [[nodiscard]] class World const& world() const;

protected:
    Object() = default;

    [[nodiscard]] virtual b2WorldId world_id() const = 0;
};

}

#endif //TOPDOWN_OBJECT_HH
