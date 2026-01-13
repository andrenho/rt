#ifndef RT_CITY_HH
#define RT_CITY_HH

#include <vector>

#include "geometry/shapes.hh"

namespace city {

struct BuildingConfig {
    size_t id;
    float  w, h;
    float  door_position = .5;

    [[nodiscard]] float size() const { return std::max(w, h); }
};

struct CityConfig {
    size_t                          id;
    int                             seed;
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
        geo::Point door_position;
    };
    size_t                  id;
    std::vector<geo::Shape> original_poisson_disks {};
    std::vector<geo::Shape> poisson_disks {};
    std::vector<Building>   buildings {};
    geo::Shape              boundary;
};

City generate_city(CityConfig const& cfg);

}

#endif //RT_CITY_HH
