#include "map.hh"

#include <random>

#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"

#include "PerlinNoise.hpp"

namespace map {

//
// POLYGONS GENERATION
//

std::vector<geo::Point> generate_points(std::mt19937& rng, size_t tiles_w, size_t tiles_h, MapConfig const& cfg)
{
    std::vector<geo::Point> polygon_points {};

    // generate points
    for (size_t x = 1; x < tiles_w; ++x) {
        for (size_t y = 1; y < tiles_h; ++y) {
            polygon_points.emplace_back(x * cfg.point_density, y * cfg.point_density);
        }
    }

    // add randomness
    std::uniform_real_distribution<float> distances(0.0, ((float) cfg.point_density) * cfg.point_randomness);
    std::uniform_real_distribution<float> angles(0.0, 2.0);
    for (auto& p: polygon_points) {
        p.x += distances(rng) * (float) cos(angles(rng));
        p.y += distances(rng) * (float) sin(angles(rng));
    }

    return polygon_points;
}

std::vector<Biome> generate_biome_tiles(std::vector<geo::Point> const& points)
{
    std::vector<Biome> biomes;

    jcv_diagram diagram {};
    std::vector<jcv_point> jcv_points; jcv_points.reserve(points.size());
    for (auto const& p: points)
        jcv_points.emplace_back(p.x, p.y);

    jcv_diagram_generate((int) jcv_points.size(), jcv_points.data(), nullptr, nullptr, &diagram);

    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    for(int i = 0; i < diagram.numsites; ++i) {
        geo::Polygon polygon;

        const jcv_site* site = &sites[i];

        const jcv_graphedge* e = site->edges;
        while (e) {
            polygon.emplace_back(e->pos[0].x, e->pos[0].y);
            e = e->next;
        }

        biomes.emplace_back(geo::Point { jcv_points[i].x, jcv_points[i].y },
                polygon.center(), std::move(polygon), .5f, Biome::Type::Unknown);
    }

    jcv_diagram_free(&diagram);

    return biomes;
}

std::vector<geo::Point> relax_points(std::vector<Biome> const& biomes)
{
    std::vector<geo::Point> polygon_points;
    for (auto const& biome: biomes)
        polygon_points.emplace_back(biome.center_point);
    return polygon_points;
}

//
// TERRAIN GENERATION
//

void update_biome_elevation(std::vector<Biome>& biomes, MapConfig const& cfg)
{
    const siv::PerlinNoise::seed_type seed = cfg.seed;
    const siv::PerlinNoise perlin(seed);

    for (auto& biome: biomes) {
        auto p = biome.center_point;
        float w = (float) cfg.map_w;
        float h = (float) cfg.map_h;
        float distance_from_center = ((p.x-w*0.5f)*(p.x-w*0.5f)+(p.y-h*0.5f)*(p.y-h*0.5f))/((w*0.5f)*(w*0.5f)+(h*0.5f)*(h*0.5f)) / .5f;
        float r = (float) perlin.octave2D_01(p.x / (float) cfg.map_w * 2, p.y / (float) cfg.map_h * 2, 4);
        biome.elevation = std::clamp(1.f - r * distance_from_center / .5f, .0f, 1.f);
    }
}

void update_biome_moisture(std::vector<Biome>& biomes, MapConfig const& cfg)
{
    const siv::PerlinNoise::seed_type seed = cfg.seed + 1;
    const siv::PerlinNoise perlin(seed);

    for (auto& biome: biomes) {
        auto p = biome.center_point;
        biome.moisture = (float) perlin.octave2D_01(p.x / (float) cfg.map_w * 2, p.y / (float) cfg.map_h * 2, 16);
    }
}

void update_biome_ocean(std::vector<Biome>& biomes, MapConfig const& cfg)
{
    for (auto& biome: biomes)
        if (biome.elevation < cfg.ocean_elevation)
            biome.type = Biome::Ocean;
}

void add_lakes(std::vector<Biome>& biomes, MapConfig const& cfg)
{
    const siv::PerlinNoise::seed_type seed = cfg.seed + 2;
    const siv::PerlinNoise perlin(seed);

    for (auto& biome: biomes) {
        auto p = biome.center_point;
        if (perlin.octave2D_01(p.x / (float) cfg.map_w * 2, p.y / (float) cfg.map_h * 2, 4) < cfg.lake_threshold)
            biome.type = Biome::Ocean;
    }
}

void update_terrain_type(std::vector<Biome>& biomes, MapConfig const& cfg)
{
    for (auto& biome: biomes) {
        auto p = biome.center_point;
        if (biome.type != Biome::Type::Unknown)
            continue;;
        if (biome.elevation > .98f) {
            biome.type = Biome::Type::Snow;
        } else if (biome.elevation > .8f) {
            if (biome.moisture < .25f)
                biome.type = Biome::Type::Desert;
            else if (biome.moisture < .5f)
                biome.type = Biome::Type::Tundra;
            else if (biome.moisture < .75f)
                biome.type = Biome::Type::Grassland;
            else
                biome.type = Biome::Type::PineForest;
        } else {
            if (biome.moisture < .25f)
                biome.type = Biome::Type::Desert;
            else if (biome.moisture < .5f)
                biome.type = Biome::Type::Savannah;
            else if (biome.moisture < .75f)
                biome.type = Biome::Type::Forest;
            else
                biome.type = Biome::Type::RainForest;
        }
    }
}

//
// CITIES
//

void find_city_locations(std::vector<Biome>& biomes, MapConfig const& cfg, std::mt19937& rng)
{
    size_t initial_city_count = (cfg.number_of_cities * 2.f);  // double cities as some will end up in water

    // create distributed points with random component
    size_t cities_h = (size_t) std::round(std::sqrt(initial_city_count * cfg.map_h / cfg.map_w));
    size_t cities_w = (size_t) ceil(initial_city_count / cities_h);
    float diff_x = cfg.map_w / cities_w / 2.f;
    float diff_y = cfg.map_h / cities_h / 2.f;

    std::uniform_real_distribution<float> distances_x(0.0, diff_x * 2);
    std::uniform_real_distribution<float> distances_y(0.0, diff_y * 2);
    std::uniform_real_distribution<float> angles(0.0, 2.0);

    std::vector<jcv_point> jcv_points; jcv_points.reserve(initial_city_count);
    for (float x = 0; x < cities_w; ++x) {
        for (float y = 0; y < cities_h; ++y) {
            float px = x * (cfg.map_w / cities_w) + diff_x + distances_x(rng) * (float) cos(angles(rng));
            float py = y * (cfg.map_h / cities_h) + diff_y + distances_y(rng) * (float) sin(angles(rng));
            jcv_points.emplace_back(px, py);
        }
    }

    // create voronoi
    jcv_diagram diagram {};
    jcv_diagram_generate((int) jcv_points.size(), jcv_points.data(), nullptr, nullptr, &diagram);

    // relax voronoi and create list of points
    std::vector<geo::Point> points;
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    for(int i = 0; i < diagram.numsites; ++i) {
        const jcv_site* site = &sites[i];

        const jcv_graphedge* e = site->edges;
        float count = 0;
        float sum_x = 0.f, sum_y = 0.f;
        while (e) {
            sum_x += e->pos[0].x;
            sum_y += e->pos[0].y;
            e = e->next;
            ++count;
        }

        if (count > 0)
            points.emplace_back(sum_x / count, sum_y / count);
    }

    jcv_diagram_free(&diagram);

    // find biomes
    std::shuffle(points.begin(), points.end(), rng);
    size_t count = 0;
    for (auto& biome: biomes) {
        for (auto const& p: points) {
            if (geo::contains_point(biome.polygon, p)) {
                if (biome.type != Biome::Ocean) {
                    biome.contains_city = true;
                    if ((++count) >= cfg.number_of_cities)
                        return;
                }
            }
        }
    }
}

//
// PUBLIC FUNCTIONS
//

MapOutput create(MapConfig const& cfg)
{
    std::mt19937 rng(cfg.seed);

    MapOutput output {
        .w = (size_t) cfg.map_w,
        .h = (size_t) cfg.map_h,
    };

    size_t tiles_w = (size_t) (cfg.map_w / cfg.point_density);
    size_t tiles_h = (size_t) (cfg.map_h / cfg.point_density);

    std::vector<geo::Point> polygon_points = generate_points(rng, tiles_w, tiles_h, cfg);

    int relaxations = cfg.polygon_relaxation_steps;
generate_polygons_again:
    std::vector<Biome> biomes = generate_biome_tiles(polygon_points);
    if (relaxations > 0) {
        --relaxations;
        polygon_points = relax_points(biomes);
        goto generate_polygons_again;
    }

    update_biome_elevation(biomes, cfg);
    update_biome_moisture(biomes, cfg);

    update_biome_ocean(biomes, cfg);
    add_lakes(biomes, cfg);

    update_terrain_type(biomes, cfg);

    find_city_locations(biomes, cfg, rng);

    output.biomes = std::move(biomes);
    return output;
}

} // map
