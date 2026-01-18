#ifndef RT_CITY_HH
#define RT_CITY_HH

#include <optional>
#include <vector>

#include "geometry/shapes.hh"

namespace city {

struct BuildingConfig {
    size_t id;
    float  w, h;

    struct Entrance {
        float position = .5;
        float entrance_sz = 1.5f;
        float wall_width = 2.f;
    };
    std::optional<Entrance> entrance = Entrance();

    [[nodiscard]] float size() const { return std::max(w, h); }
};

struct CityConfig {
    size_t                          id;
    std::vector<geo::Shape>         obstacles;
    geo::Point                      center { 0, 0 };
    std::vector<BuildingConfig>     buildings;
    float                           max_size;
    float                           angle_variation;
    std::variant<float, geo::Point> city_direction;
    float                           boundary_size;
};

struct City {
    struct Building {
        size_t     id;
        geo::Shape shape;
        std::vector<geo::Shape> walls;
        std::optional<geo::Shape> entrance_sensor;
        geo::Point door_position;
    };
    size_t                  id;
    std::vector<geo::Shape> original_poisson_disks {};
    std::vector<geo::Shape> poisson_disks {};
    std::vector<Building>   buildings {};
    geo::Shape              boundary;
};

City generate_city(CityConfig const& cfg, Random& random);

}

#endif //RT_CITY_HH
