#include "physicalmap.hh"

#include <optional>

namespace map {

static constexpr float ROAD_WIDTH = 8.f;

struct TerrainDef {
    bool                 passable;
    std::optional<float> static_features_distance {};
};
static const std::unordered_map<Biome::Type, TerrainDef> terrain_def = {
        { Biome::Type::Ocean,      { false, {} } },
        { Biome::Type::Snow,       { true,  40.f } },
        { Biome::Type::Tundra,     { true,  80.f } },
        { Biome::Type::Desert,     { true,  80.f } },
        { Biome::Type::Grassland,  { true,  80.f } },
        { Biome::Type::Savannah,   { true,  30.f } },
        { Biome::Type::PineForest, { false, 15.f } },
        { Biome::Type::Forest,     { true,  10.f } },
        { Biome::Type::RainForest, { false, 15.f, } },
};

static std::unordered_map<geo::Point, uint8_t> static_features(geo::Shape const& shape, float distance, std::mt19937 rng)
{
    std::unordered_map<geo::Point, uint8_t> features;

    auto points = geo::Point::poisson(shape, distance, 0, 4);

    std::uniform_int_distribution<uint8_t> random_data;
    for (auto const& point: points)
        features[point] = random_data(rng);

    return features;
}

static void add_terrain(std::unique_ptr<Biome> const& biome, PhysicalMap& pmap, std::mt19937 rng)
{
    size_t hash = std::hash<geo::Shape>()(biome->polygon);

    PhysicalMap::Object object {
        .shape = biome->polygon,
        .type = PhysicalMap::Object::Type::Terrain,
        .terrain_type = biome->type,
    };

    TerrainDef const& def = terrain_def.at(biome->type);
    if (def.static_features_distance)
        object.static_features = static_features(biome->polygon, *def.static_features_distance, rng);

    pmap.objects[hash] = std::move(object);
}

PhysicalMap generate_physical_map(Map const& map, size_t seed)
{
    PhysicalMap pmap;
    std::mt19937 rng(seed);

    pmap.w = map.w;
    pmap.h = map.h;

    // roads
    for (auto const& road_segment: map.road_segments) {
        geo::Shape shape = geo::Shape::Capsule(road_segment.first, road_segment.second, ROAD_WIDTH);
        size_t hash = std::hash<geo::Shape>()(shape);
        pmap.objects[hash] = {
            .shape = shape,
            .type = PhysicalMap::Object::Type::Road,
        };
    }

    // terrains
    for (auto const& biome: map.biomes)
        add_terrain(biome, pmap, rng);

    return pmap;
}

} // map
