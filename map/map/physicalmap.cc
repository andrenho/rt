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

static std::unordered_map<geo::Point, uint8_t> static_features(geo::Shape const& shape, float distance, Random& random)
{
    std::unordered_map<geo::Point, uint8_t> features;

    auto points = geo::Point::poisson(shape, distance, 0, 4);

    for (auto const& point: points)
        features[point] = random.next_uint8();

    return features;
}

static void add_terrain(std::unique_ptr<Biome> const& biome, PhysicalMap& pmap, Random& random)
{
    size_t hash = std::hash<geo::Shape>()(biome->polygon);

    PhysicalMap::Object object {
        .shape = biome->polygon,
        .type = PhysicalMap::Object::Type::Terrain,
        .terrain_type = biome->type,
    };

    TerrainDef const& def = terrain_def.at(biome->type);
    if (def.static_features_distance)
        object.static_features = static_features(biome->polygon, *def.static_features_distance, random);

    pmap.objects[hash] = std::move(object);
}

void create_quadrants(PhysicalMap& pmap, size_t quadrant_sz)
{
    pmap.quads_w = pmap.w / quadrant_sz;
    pmap.quads_h = pmap.h / quadrant_sz;

    for (int x = 0; x < pmap.quads_w; x++) {
        for (int y = 0; y < pmap.quads_h; y++) {
            geo::Bounds bounds {
                { (x-1) * (int) quadrant_sz, (y-1) * (int) quadrant_sz },
                { (x+2) * (int) quadrant_sz, (y+2) * (int) quadrant_sz }
            };

            std::vector<size_t> obj_ids;
            for (auto const& [hash, obj]: pmap.objects) {
                if (obj.shape.aabb_intersects(bounds))
                    obj_ids.push_back(hash);
            }
            pmap.quadrants[geo::UPoint(x, y)] = std::move(obj_ids);
        }
    }
}

std::vector<city::BuildingConfig> city_buildings(CitySize city_size, size_t* building_id)
{
#define BD_SMALL  { .id = (*building_id)++, .w = 10, .h = 10 }
#define BD_MEDIUM { .id = (*building_id)++, .w = 15, .h = 12 }
#define BD_LARGE  { .id = (*building_id)++, .w = 20, .h = 15 }
#define BD_HUGE   { .id = (*building_id)++, .w = 25, .h = 18, .entrance = city::BuildingConfig::Entrance { .position = .8f } }

    switch (city_size) {
        case CitySize::TradingPost:
            return {
                BD_SMALL, BD_SMALL, BD_SMALL,
                BD_MEDIUM, BD_MEDIUM
            };
        case CitySize::Village:
            return {
                BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL,
                BD_MEDIUM, BD_MEDIUM, BD_MEDIUM,
                BD_LARGE
            };
        case CitySize::Town:
            return {
                BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL,
                BD_MEDIUM, BD_MEDIUM, BD_MEDIUM, BD_MEDIUM,
                BD_LARGE, BD_LARGE,
            };
        case CitySize::City:
            return {
                BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL, BD_SMALL,
                BD_MEDIUM, BD_MEDIUM, BD_MEDIUM, BD_MEDIUM, BD_MEDIUM, BD_MEDIUM,
                BD_LARGE, BD_LARGE, BD_LARGE,
                BD_HUGE, BD_HUGE
            };
    }
    abort();

#undef BD_SMALL
#undef BD_MEDIUM
#undef BD_LARGE
#undef BD_HUGE
}

void add_city(City const& city_, size_t id, size_t* building_id, std::vector<geo::Shape> const& obstacles, PhysicalMap* pmap, geo::Shape* city_area, Random& random)
{
    // generate city layout
    city::CityConfig cfg {
        .id = id,
        .obstacles = obstacles,
        .center = city_.location,
        .buildings = city_buildings(city_.size, building_id),
        .max_size = 300,
        .angle_variation = .7f,
        .city_direction = city_.location,
        .boundary_size = 20.f,
    };
    auto gcity = city::generate_city(cfg, random);

    // add to physical map
    *city_area = gcity.boundary;

    for (auto const& building: gcity.buildings) {
        for (auto const& wall: building.walls) {
            size_t hash = std::hash<geo::Shape>()(wall);
            pmap->objects[hash] = {
                .shape = wall,
                .type = PhysicalMap::Object::Type::Wall,
            };
        }

        if (building.sensor) {
            size_t hash = std::hash<geo::Shape>()(*building.sensor);
            pmap->objects[hash] = {
                .shape = *building.sensor,
                .type = PhysicalMap::Object::Type::Sensor,
                .sensor_id = building.id,
            };
        };
    }
}

PhysicalMap generate_physical_map(Map const& map, size_t quadrant_sz, Random& random)
{
    PhysicalMap pmap;

    pmap.w = map.w;
    pmap.h = map.h;

    // roads
    std::vector<geo::Shape> all_obstacles;
    for (auto const& road_segment: map.road_segments) {
        geo::Shape shape = geo::Shape::Capsule(road_segment.first, road_segment.second, ROAD_WIDTH);
        all_obstacles.push_back(shape);
        size_t hash = std::hash<geo::Shape>()(shape);
        pmap.objects[hash] = {
            .shape = std::move(shape),
            .type = PhysicalMap::Object::Type::Road,
        };
    }

    // create cities
    std::vector<geo::Shape> city_areas;
    size_t id = 0, building_id = 0;
    for (auto const& city: map.cities) {
        geo::Shape city_area;
        add_city(*city, id++, &building_id, all_obstacles, &pmap, &city_area, random);
        city_areas.emplace_back(std::move(city_area));
    }

    // terrains
    for (auto const& biome: map.biomes)
        add_terrain(biome, pmap, random);

    // create quadrants
    create_quadrants(pmap, quadrant_sz);

    return pmap;
}

} // map
