#include "map.hh"

#include <random>

#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"

#include "PerlinNoise.hpp"

#include "fmt/core.h"
#include "graaflib/graph.h"
#include "graaflib/algorithm/minimum_spanning_tree/kruskal.h"
#include "graaflib/algorithm/shortest_path/dijkstra_shortest_path.h"

namespace map {

//
// POLYGONS GENERATION
//

static std::vector<geo::Point> generate_points(std::mt19937& rng, size_t tiles_w, size_t tiles_h, MapConfig const& cfg)
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

static std::vector<std::unique_ptr<Biome>> generate_biome_tiles(std::vector<geo::Point> const& points)
{
    std::vector<std::unique_ptr<Biome>> biomes;

    jcv_diagram diagram {};
    std::vector<jcv_point> jcv_points; jcv_points.reserve(points.size());
    for (auto const& p: points)
        jcv_points.emplace_back(p.x, p.y);

    jcv_diagram_generate((int) jcv_points.size(), jcv_points.data(), nullptr, nullptr, &diagram);

    const jcv_site* sites = jcv_diagram_get_sites(&diagram);

    std::unordered_map<Biome*, std::vector<jcv_site*>> biome_neighbour_sites;
    std::unordered_map<jcv_site const*, Biome*> sites_biomes;

    // create biomes
    for(int i = 0; i < diagram.numsites; ++i) {
        geo::Polygon polygon;
        std::vector<jcv_site*> neighbours;

        const jcv_site* site = &sites[i];

        const jcv_graphedge* e = site->edges;
        while (e) {
            polygon.emplace_back(e->pos[0].x, e->pos[0].y);
            neighbours.emplace_back(e->neighbor);
            e = e->next;
        }

        auto& biome = biomes.emplace_back(std::make_unique<Biome>(geo::Point { jcv_points[i].x, jcv_points[i].y },
                polygon.center(), std::move(polygon), .5f, Biome::Type::Unknown));
        biome_neighbour_sites[biome.get()] = std::move(neighbours);
        sites_biomes[site] = biome.get();
    }

    // find neighbours
    for (auto& biome: biomes)
        for (jcv_site* site: biome_neighbour_sites.at(biome.get()))
            if (site)
                biome->neighbours.push_back(sites_biomes.at(site));

    jcv_diagram_free(&diagram);

    return biomes;
}

static std::vector<geo::Point> relax_points(std::vector<std::unique_ptr<Biome>> const& biomes)
{
    std::vector<geo::Point> polygon_points;
    for (auto const& biome: biomes)
        polygon_points.emplace_back(biome->center_point);
    return polygon_points;
}

//
// TERRAIN GENERATION
//

static void update_biome_elevation(std::vector<std::unique_ptr<Biome>>& biomes, MapConfig const& cfg)
{
    const siv::PerlinNoise::seed_type seed = cfg.seed;
    const siv::PerlinNoise perlin(seed);

    for (auto& biome: biomes) {
        auto p = biome->center_point;
        float w = (float) cfg.map_w;
        float h = (float) cfg.map_h;
        float distance_from_center = ((p.x-w*0.5f)*(p.x-w*0.5f)+(p.y-h*0.5f)*(p.y-h*0.5f))/((w*0.5f)*(w*0.5f)+(h*0.5f)*(h*0.5f)) / .5f;
        float r = (float) perlin.octave2D_01(p.x / (float) cfg.map_w * 2, p.y / (float) cfg.map_h * 2, 4);
        biome->elevation = std::clamp(1.f - r * distance_from_center / .5f, .0f, 1.f);
    }
}

static void update_biome_moisture(std::vector<std::unique_ptr<Biome>>& biomes, MapConfig const& cfg)
{
    const siv::PerlinNoise::seed_type seed = cfg.seed + 1;
    const siv::PerlinNoise perlin(seed);

    for (auto& biome: biomes) {
        auto p = biome->center_point;
        biome->moisture = (float) perlin.octave2D_01(p.x / (float) cfg.map_w * 2, p.y / (float) cfg.map_h * 2, 16);
    }
}

static void update_biome_ocean(std::vector<std::unique_ptr<Biome>>& biomes, MapConfig const& cfg)
{
    for (auto& biome: biomes)
        if (biome->elevation < cfg.ocean_elevation)
            biome->type = Biome::Ocean;
}

static void add_lakes(std::vector<std::unique_ptr<Biome>>& biomes, MapConfig const& cfg)
{
    const siv::PerlinNoise::seed_type seed = cfg.seed + 2;
    const siv::PerlinNoise perlin(seed);

    for (auto& biome: biomes) {
        auto p = biome->center_point;
        if (perlin.octave2D_01(p.x / (float) cfg.map_w * 2, p.y / (float) cfg.map_h * 2, 4) < cfg.lake_threshold)
            biome->type = Biome::Ocean;
    }
}

static void update_terrain_type(std::vector<std::unique_ptr<Biome>>& biomes)
{
    for (auto& biome: biomes) {
        if (biome->type != Biome::Type::Unknown)
            continue;;
        if (biome->elevation > .98f) {
            biome->type = Biome::Type::Snow;
        } else if (biome->elevation > .8f) {
            if (biome->moisture < .25f)
                biome->type = Biome::Type::Desert;
            else if (biome->moisture < .5f)
                biome->type = Biome::Type::Tundra;
            else if (biome->moisture < .75f)
                biome->type = Biome::Type::Grassland;
            else
                biome->type = Biome::Type::PineForest;
        } else {
            if (biome->moisture < .25f)
                biome->type = Biome::Type::Desert;
            else if (biome->moisture < .5f)
                biome->type = Biome::Type::Savannah;
            else if (biome->moisture < .75f)
                biome->type = Biome::Type::Forest;
            else
                biome->type = Biome::Type::RainForest;
        }
    }
}

//
// CITIES
//

static std::vector<std::unique_ptr<City>> create_cities(std::vector<std::unique_ptr<Biome>> const& biomes, MapConfig const& cfg, std::mt19937& rng, size_t city_count)
{
    std::vector<std::unique_ptr<City>> cities;

    // create distributed points with random component
    size_t cities_h = (size_t) std::round(std::sqrt(city_count * cfg.map_h / cfg.map_w));
    size_t cities_w = (size_t) ceil(city_count / cities_h);
    float diff_x = cfg.map_w / cities_w / 2.f;
    float diff_y = cfg.map_h / cities_h / 2.f;

    std::uniform_real_distribution<float> distances_x(0.0, diff_x * 2);
    std::uniform_real_distribution<float> distances_y(0.0, diff_y * 2);
    std::uniform_real_distribution<float> angles(0.0, 2.0);

    std::vector<jcv_point> jcv_points; jcv_points.reserve(city_count);
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
    std::vector<int> biome_n(biomes.size()); std::iota(biome_n.begin(), biome_n.end(), 0); std::shuffle(biome_n.begin(), biome_n.end(), rng);
    for (auto i: biome_n) {
        for (auto const& p: points) {
            if (geo::contains_point(biomes.at(i)->polygon, p)) {
                if (biomes.at(i)->type != Biome::Ocean) {
                    cities.emplace_back(std::make_unique<City>(biomes.at(i).get(), biomes.at(i)->polygon.center()));
                    if (cities.size() >= cfg.number_of_cities)
                        goto done;
                }
            }
        }
    }

done:
    return cities;
}

static std::vector<std::unique_ptr<City>> find_city_locations(std::vector<std::unique_ptr<Biome>>& biomes, MapConfig const& cfg, std::mt19937& rng)
{
    size_t city_count = cfg.number_of_cities + 10;
    std::vector<std::unique_ptr<City>> cities;

    // create city list
    do {
        cities = create_cities(biomes, cfg, rng, city_count);
        city_count += 10;
        if (city_count > cfg.number_of_cities * 3)   // sanity check
            break;
    } while (cities.size() < cfg.number_of_cities);  // if not enough cities were created, try again with more cities

    for (auto& city: cities)
        city->biome->contains_city = true;

    return cities;
}

//
// CITY CONNECTIONS
//

static void find_minimally_connected_cities(std::vector<std::unique_ptr<City>>& cities)
{
    // create graph
    std::unordered_map<City*, graaf::vertex_id_t> vertices;
    graaf::undirected_graph<City*, int> g;
    for (auto& city: cities)
        vertices[city.get()] = g.add_vertex(city.get());

    // add graph weights
    for (auto const& city1: cities) {
        for (auto const& city2: cities) {
            int sq_distance = (int) (std::pow(city2->location.x - city1->location.x, 2) + std::pow(city2->location.y - city1->location.y, 2));
            g.add_edge(vertices.at(city1.get()), vertices.at(city2.get()), sq_distance);
        }
    }

    // run algorithm (Kruskal)
    auto edges = graaf::algorithm::kruskal_minimum_spanning_tree(g);

    // connect cities based on algorithm result
    for (auto const& edge_id: edges) {
        auto city1 = g.get_vertex(edge_id.first);
        auto city2 = g.get_vertex(edge_id.second);
        city1->connected_cities.emplace(city2);
    }
}

static void find_connected_cities(std::vector<std::unique_ptr<City>>& cities, MapConfig const& cfg)
{
    // find minimal set of connected cities
    find_minimally_connected_cities(cities);

    // connect all cities that have a minimal distance
    const float max_distance_sq = std::pow(cfg.connect_city_distance, 2);
    for (auto& city: cities) {
        for (auto const& other_city: cities) {
            float sq_distance = std::pow(other_city->location.x - city->location.x, 2) + std::pow(other_city->location.y - city->location.y, 2);
            if (sq_distance < max_distance_sq)
                city->connected_cities.emplace(other_city.get());
        }
    }

    // remove duplicates
    for (auto& city: cities) {
        for (auto& other_city: cities) {
            if (city->connected_cities.contains(other_city.get()))
                other_city->connected_cities.erase(city.get());
        }
    }
}

//
// BUILD ROADS
//

static std::vector<RoadSegment> build_road_segments(std::vector<std::unique_ptr<Biome>> const& biomes, std::vector<std::unique_ptr<City>> const& cities, MapConfig const& cfg)
{
    std::vector<RoadSegment> road_segments;

    // create graph
    graaf::undirected_graph<Biome*, float> g;
    std::unordered_map<Biome*, graaf::vertex_id_t> vertices;
    std::unordered_map<graaf::vertex_id_t, Biome*> biome_vert;
    for (auto const& biome: biomes) {
        graaf::vertex_id_t v = g.add_vertex(biome.get());
        vertices[biome.get()] = v;
        biome_vert[v] = biome.get();
    }

    // add weights
    for (auto const& biome1: biomes) {
        for (auto const& biome2: biome1->neighbours) {
            float weight = 1.f;
            switch (biome2->type) {
                case Biome::Ocean:
                    weight = cfg.road_weight_ocean;
                    break;
                case Biome::PineForest:
                case Biome::Forest:
                case Biome::RainForest:
                    weight = cfg.road_weight_forest;
                    break;
                default: break;
            }
            g.add_edge(vertices.at(biome1.get()), vertices.at(biome2), weight);
        }
    }

    // calculate roads
    for (auto const& city1: cities) {
        for (auto const& city2: city1->connected_cities) {
            // calculate best path
            auto result = graaf::algorithm::dijkstra_shortest_path(g, vertices.at(city1->biome), vertices.at(city2->biome));

            // create road segment
            if (result.has_value()) {
                std::optional<Biome*> last {};
                graaf::vertex_id_t last_v_id = -1;
                for (auto v_id: result->vertices) {
                    Biome* nw = biome_vert.at(v_id);
                    if (last) {
                        // add road
                        road_segments.emplace_back(last.value()->center_point, nw->center_point);

                        // adjust edge to indicate that there's a road here now, and we rather reuse it
                        g.remove_edge(last_v_id, v_id);
                        g.add_edge(last_v_id, v_id, cfg.road_weight_reuse);
                    }
                    last = nw;
                    last_v_id = v_id;
                }
            }
        }
    }

    return road_segments;
}

//
// PUBLIC FUNCTIONS
//

Map create(MapConfig const& cfg)
{
    std::mt19937 rng(cfg.seed);

    Map output {
        .w = (size_t) cfg.map_w,
        .h = (size_t) cfg.map_h,
    };

    size_t tiles_w = (size_t) (cfg.map_w / cfg.point_density);
    size_t tiles_h = (size_t) (cfg.map_h / cfg.point_density);

    std::vector<geo::Point> polygon_points = generate_points(rng, tiles_w, tiles_h, cfg);

    int relaxations = cfg.polygon_relaxation_steps;
generate_polygons_again:
    std::vector<std::unique_ptr<Biome>> biomes = generate_biome_tiles(polygon_points);
    if (relaxations > 0) {
        --relaxations;
        polygon_points = relax_points(biomes);
        goto generate_polygons_again;
    }

    update_biome_elevation(biomes, cfg);
    update_biome_moisture(biomes, cfg);

    update_biome_ocean(biomes, cfg);
    add_lakes(biomes, cfg);

    update_terrain_type(biomes);

    auto cities = find_city_locations(biomes, cfg, rng);
    find_connected_cities(cities, cfg);

    std::vector<RoadSegment> road_segments = build_road_segments(biomes, cities, cfg);

    output.biomes = std::move(biomes);
    output.cities = std::move(cities);
    output.road_segments = std::move(road_segments);
    return output;
}

} // map
