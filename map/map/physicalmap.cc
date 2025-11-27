#include "physicalmap.hh"

static constexpr float ROAD_WIDTH = 8.f;

namespace map {

static void add_static_features(geo::Polygon const& polygon, float prop, PhysicalMap& pmap)
{
}

static void add_terrain(std::unique_ptr<Biome> const& biome, PhysicalMap& pmap)
{
    PhysicalMap::Object object;
    object.shape = biome->polygon;
    object.additional_info = biome->type;

    switch (biome->type) {
        case Biome::Ocean:
            object.type = PhysicalMap::ObjectType::Water;
            break;
        case Biome::Snow:
            object.type = PhysicalMap::ObjectType::TerrainSensor;
            add_static_features(biome->polygon, 0.1f, pmap);
            break;
        case Biome::Tundra:
            object.type = PhysicalMap::ObjectType::TerrainSensor;
            break;
        case Biome::Desert:
            object.type = PhysicalMap::ObjectType::TerrainSensor;
            add_static_features(biome->polygon, 0.05f, pmap);
            break;
        case Biome::Grassland:
            object.type = PhysicalMap::ObjectType::TerrainSensor;
            add_static_features(biome->polygon, 0.05f, pmap);
            break;
        case Biome::Savannah:
            object.type = PhysicalMap::ObjectType::TerrainSensor;
            add_static_features(biome->polygon, 0.3f, pmap);
            break;
        case Biome::PineForest:
            object.type = PhysicalMap::ObjectType::ImpassableTerrain;
            break;
        case Biome::Forest:
            object.type = PhysicalMap::ObjectType::TerrainSensor;
            add_static_features(biome->polygon, 0.8f, pmap);
            break;
        case Biome::RainForest:
            object.type = PhysicalMap::ObjectType::ImpassableTerrain;
            break;
        case Biome::Unknown:
            break;
    }

    pmap.objects.emplace_back(std::move(object));
}

PhysicalMap generate_physical_map(Map const& map)
{
    PhysicalMap pmap;

    // roads
    for (auto const& road_segment: map.road_segments) {
        pmap.objects.emplace_back(
                PhysicalMap::ObjectType::Road,
                geo::Capsule { road_segment.first, road_segment.second, ROAD_WIDTH });
    }

    // terrains
    for (auto const& biome: map.biomes)
        add_terrain(biome, pmap);

    return pmap;
}

} // map
