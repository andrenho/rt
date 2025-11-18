#ifndef TOPDOWN_MISSILE_HH
#define TOPDOWN_MISSILE_HH

#include "explosive.hh"

namespace topdown {

class Missile : public Explosive {
public:
    Missile(class World const& world, Object* originator, b2Vec2 target, float speed, ExplosiveDef const& explosive_def);

    [[nodiscard]] Category category() const override { return Category::Missile; }
    [[nodiscard]] std::vector<Category> categories_contact() const override { return { Category::Dynamic, Category::Solid }; }

    void step() override;

private:
    b2Vec2 target_;
    b2Vec2 initial_pos_;
    float  distance_sq_target_;
};

}

#endif //TOPDOWN_MISSILE_HH
