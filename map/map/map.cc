#include "map.hh"

#include <random>

namespace map {

void Map::initialize(MapConfig const& cfg)
{
    cfg_ = cfg;

    rng_ = std::mt19937(cfg.seed);

    polygon_points.clear();

    generate_points();
}

void Map::generate_points()
{
    // generate points
    int w = cfg_.map_w / cfg_.point_density;
    int h = cfg_.map_h / cfg_.point_density;
    for (int x = 1; x < w; ++x)
        for (int y = 1; y < h; ++y)
            polygon_points.emplace_back(x * cfg_.point_density, y * cfg_.point_density);

    // add randomness

}

} // map
