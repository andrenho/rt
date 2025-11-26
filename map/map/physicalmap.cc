#include "physicalmap.hh"

static constexpr float ROAD_WIDTH = 8.f;

namespace map {

PhysicalMap generate_physical_map(Map const& map)
{
    PhysicalMap pmap;

    // roads
    for (auto const& road_segment: map.road_segments) {
        pmap.objects.emplace_back(
                PhysicalMap::ObjectType::Road,
                geo::Capsule { road_segment.first, road_segment.second, ROAD_WIDTH });
    }

    return pmap;
}

} // map
