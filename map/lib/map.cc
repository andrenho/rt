#include "map.hh"

namespace map {

Map::Map(MapConfig const& cfg)
    : bounds_({ { -20000, -20000 }, { 20000, 20000 } })
{
    terrains_.emplace_back(Terrain::Type::Dirt, Box({ -100, -100 }, { 100, 100 }));
}

} // map
