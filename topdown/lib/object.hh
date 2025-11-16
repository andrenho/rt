#ifndef TOPDOWN_OBJECT_HH
#define TOPDOWN_OBJECT_HH

#include <limits>
#include <optional>

#include "category.hh"
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

    virtual void setup() = 0;
    virtual void schedule_myself_for_deletion();

    [[nodiscard]] Cast cast(b2Vec2 target, float max_distance=std::numeric_limits<float>::infinity()) const;

    class Explosive* fire_missile(b2Vec2 target, float speed, ExplosiveDef const& explosive_def);
    class Explosive* place_explosive(ExplosiveDef const& explosive_def);

    [[nodiscard]] virtual b2Vec2 center() const = 0;

    [[nodiscard]] class World& world();
    [[nodiscard]] class World const& world() const;

    [[nodiscard]] virtual Category category() const = 0;
    [[nodiscard]] virtual bool explodes_on_contact() const { return false; }

protected:
    Object() = default;

    void setup_collisions(b2ShapeId shape);

    [[nodiscard]] virtual b2WorldId world_id() const = 0;
    [[nodiscard]] virtual std::vector<Category> categories_contact() const = 0;
};

}

#endif //TOPDOWN_OBJECT_HH
