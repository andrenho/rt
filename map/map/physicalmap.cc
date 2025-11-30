#include "physicalmap.hh"

#include <optional>

namespace map {

static constexpr float ROAD_WIDTH = 8.f;

struct TerrainDef {
    bool                 passable;
    std::optional<float> static_features_prop {};
};
static const std::unordered_map<Biome::Type, TerrainDef> terrain_def = {
        { Biome::Type::Snow,       { true,  0.1f } },
        { Biome::Type::Tundra,     { true,  0.05f } },
        { Biome::Type::Desert,     { true,  0.05f } },
        { Biome::Type::Grassland,  { true,  0.05f } },
        { Biome::Type::Savannah,   { true,  0.3f } },
        { Biome::Type::PineForest, { false, 1.f } },
        { Biome::Type::Forest,     { true,  0.8f } },
        { Biome::Type::RainForest, { false, 1.f, } },
};

static std::unordered_map<geo::Point, uint8_t> static_features(geo::Shape const& shape, float prop, std::mt19937 rng)
{
    std::unordered_map<geo::Point, uint8_t> features;

    geo::Bounds bounds = shape.aabb();
    float prop_x = (bounds.bottom_right.x - bounds.top_left.x) * prop;
    float prop_y = (bounds.bottom_right.y - bounds.top_left.y) * prop;

    std::uniform_int_distribution<uint8_t> random_data;
    auto points = geo::Point::grid(shape, prop_x, prop_y, rng, .7f);
    // points = geo::Point::relax_grid(points);
    for (auto const& point: points)
        features[point] = random_data(rng);

    return features;
}

static void add_terrain(std::unique_ptr<Biome> const& biome, PhysicalMap& pmap, std::mt19937 rng)
{
    if (biome->type == Biome::Ocean) {
        pmap.water.emplace_back(biome->polygon);
        return;
    }

    TerrainDef const& def = terrain_def.at(biome->type);
    if (!def.static_features_prop)
        pmap.terrains.emplace_back(biome->polygon, def.passable, biome->type);
    else
        pmap.terrains.emplace_back(biome->polygon, def.passable, biome->type, static_features(biome->polygon, *def.static_features_prop, rng));
}

PhysicalMap generate_physical_map(Map const& map, size_t seed)
{
    PhysicalMap pmap;
    std::mt19937 rng(seed);

    pmap.w = map.w;
    pmap.h = map.h;

    // roads
    for (auto const& road_segment: map.road_segments)
        pmap.roads.emplace_back(geo::Shape::Capsule(road_segment.first, road_segment.second, ROAD_WIDTH));

    // terrains
    for (auto const& biome: map.biomes)
        add_terrain(biome, pmap, rng);

    return pmap;
}

} // map
