#ifndef RT_CITY_HH
#define RT_CITY_HH

#include <vector>

#include "geometry/shapes.hh"

namespace city {

struct CityConfig {
    int seed;
    std::vector<geo::Shape> obstacles;
};

struct City {
    std::vector<geo::Shape> poisson_disks;
};

City generate_city(CityConfig const& cfg);

}

#endif //RT_CITY_HH
