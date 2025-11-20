#include "map.hh"

#include <random>

#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"

#include "PerlinNoise.hpp"

namespace map {

//
// POLYGONS GENERATION
//

std::vector<geo::Point> generate_points(MapConfig const& cfg, std::mt19937& rng)
{
    std::vector<geo::Point> polygon_points {};

    // generate points
    int w = cfg.map_w / cfg.point_density;
    int h = cfg.map_h / cfg.point_density;
    for (int x = 1; x < w; ++x) {
        for (int y = 1; y < h; ++y) {
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
        biome.elevation = std::clamp(1.f - (float) perlin.octave2D_01(p.x / (float) cfg.map_w * 2, p.y / (float) cfg.map_h * 2, 4) * distance_from_center / .5f, .0f, 1.f);
    }
}

void update_biome_ocean(std::vector<Biome>& biomes, MapConfig const& cfg)
{
    for (auto& biome: biomes)
        if (biome.elevation < cfg.ocean_elevation)
            biome.type = Biome::Ocean;
}

//
// PUBLIC FUNCTIONS
//

MapOutput create(MapConfig const& cfg)
{
    std::mt19937 rng(cfg.seed);

    MapOutput output { .w = (size_t) cfg.map_w, .h = (size_t) cfg.map_h };

    std::vector<geo::Point> polygon_points = generate_points(cfg, rng);

    int relaxations = cfg.polygon_relaxation_steps;
generate_polygons_again:
    std::vector<Biome> biomes = generate_biome_tiles(polygon_points);
    if (relaxations > 0) {
        --relaxations;
        polygon_points = relax_points(biomes);
        goto generate_polygons_again;
    }

    update_biome_elevation(biomes, cfg);
    update_biome_ocean(biomes, cfg);

    output.biomes = std::move(biomes);
    return output;
}

} // map
