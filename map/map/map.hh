#ifndef MAP_HH
#define MAP_HH

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

class Map {
public:
    void initialize(MapConfig const& cfg);

    [[nodiscard]] geo::Size size() const { return { (float) cfg_.map_w, (float) cfg_.map_h }; }

    std::vector<geo::Point> polygon_points {};
    std::vector<geo::Polygon> polygons {};

private:
    MapConfig cfg_ {};
    std::mt19937 rng_;

    void generate_points();
    void generate_polygons();
    void relax_points();
};

} // map

#endif //MAP_HH
