#ifndef RT_CITY_HH
#define RT_CITY_HH

#include <vector>

#include "geometry/shapes.hh"

namespace city {

struct BuildingConfig {
    float w, h;
    float door_position = .5;
    size_t count;

    [[nodiscard]] float size() const { return std::max(w, h); }
};

struct CityConfig {
    int                         seed;
    std::vector<geo::Shape>     obstacles;
    geo::Point                  center { 0, 0 };
    std::vector<BuildingConfig> buildings;
    float                       max_size;
};

struct City {
    struct Building {
        geo::Shape shape;
        geo::Point door_position;
    };
    std::vector<geo::Shape> original_poisson_disks {};
    std::vector<geo::Shape> poisson_disks {};
    std::vector<Building>   buildings {};
};

City generate_city(CityConfig const& cfg);

}

#endif //RT_CITY_HH
