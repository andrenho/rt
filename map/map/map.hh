#ifndef MAP_HH
#define MAP_HH

#include <utility>
#include <random>
#include <vector>

#include "geometry/point.hh"
#include "geometry/shapes.hh"

namespace map {

struct MapConfig {
    int    seed;
    int    map_w;
    int    map_h;
    int    point_density;
    float  point_randomness;
    int    polygon_relaxation_steps;
};

struct MapTemp {
    std::vector<geo::Point>   polygon_points {};
    std::vector<geo::Polygon> polygons {};
    std::vector<float>        polygon_heights {};
};

struct MapOutput {
    size_t w = 0, h = 0;
};

MapOutput create(MapConfig const& cfg);
std::pair<MapOutput, MapTemp> create_with_temp(MapConfig const& cfg);

} // map

#endif //MAP_HH
