#include "map.hh"

namespace map {

Map::Map(MapConfig const& map_config)
    : rng_(map_config.seed), minimap_(map_config, rng_), w(map_config.map_w), h(map_config.map_h)
{

}

}