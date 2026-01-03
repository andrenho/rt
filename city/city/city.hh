#ifndef RT_CITY_HH
#define RT_CITY_HH

#include <vector>

#include "geometry/shapes.hh"

namespace city {

struct BuildingConfig {
    float size;
    size_t count;
};

struct CityConfig {
    int                         seed;
    std::vector<geo::Shape>     obstacles;
    geo::Point                  center { 0, 0 };
    std::vector<BuildingConfig> buildings;
    float                       max_size;
};

struct City {
    std::vector<geo::Shape> original_poisson_disks {};
};

City generate_city(CityConfig const& cfg);

}

#endif //RT_CITY_HH
