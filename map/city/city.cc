#include "city.hh"

#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

namespace city {

static std::vector<geo::Shape> create_poisson_disks(CityConfig const& cfg, float max_building_sz, Random const& random)
{
    auto points = geo::Point::poisson(
            geo::Shape::Box({ cfg.center.x - cfg.max_size/2.f, cfg.center.y - cfg.max_size/2.f }, { cfg.center.x + cfg.max_size/2.f, cfg.center.y + cfg.max_size/2.f}),
            max_building_sz + 2.f, random.seed());

    auto view = points | std::views::transform([&max_building_sz](geo::Point const& p) {
        return geo::Shape::Circle(p, max_building_sz);
    });

    return { view.begin(), view.end() };
}

static std::vector<geo::Shape> remove_obstacle_overlaps(std::vector<geo::Shape> const& disks, CityConfig const& cfg)
{
    auto new_disks = disks | std::views::filter([&](geo::Shape const& disk) {
        return ranges::all_of(cfg.obstacles, [&](geo::Shape const& obstacle) { return !obstacle.expand(2.f).intersects(disk); });
    });
    return { new_disks.begin(), new_disks.end() };
}

static std::vector<City::Building> create_buildings(std::vector<BuildingConfig> const& buildings,
    std::vector<geo::Shape> const& poisson_disks, std::variant<float, geo::Point> city_direction, float angle_variation, Random& random)
{
    if (buildings.size() > poisson_disks.size())
        throw std::runtime_error("Too few poisson disks generated in city building.");

    std::vector<City::Building> r;
    for (size_t i = 0; i < std::min(buildings.size(), poisson_disks.size()); ++i) {
        BuildingConfig const& config = buildings[i];
        if (config.door_position < 0.f || config.door_position >= 1.f)
            throw std::runtime_error("'door_position' needs to be between 0 and 1");

        float angle = std::visit(overloaded {
            [&](float a) { return a; },
            [&](geo::Point const& center) { return center.angle(poisson_disks.at(i).center()); }
        }, city_direction);

        auto box = geo::Shape::Box(poisson_disks.at(i).center() - geo::Point(config.h / 2.f, config.w / 2.f),
                { config.h, config.w }, angle + random.next_float(-angle_variation, angle_variation));
        auto door_line = geo::shape::polygon_lines(std::get<geo::shape::Polygon>(box.for_visit())).at(0);
        auto door_point = geo::Point(
            door_line.p1.x + config.door_position * (door_line.p2.x - door_line.p1.x),
            door_line.p1.y + config.door_position * (door_line.p2.y - door_line.p1.y)
        );

        r.emplace_back(config.id, box, door_point);
    }
    return r;
}

City generate_city(CityConfig const& cfg, Random& random)
{
    auto max_building_sz = ranges::max_element(cfg.buildings, [](BuildingConfig const& b, BuildingConfig const& c) { return b.size() < c.size(); })->size() / 2.f;

    City city;
    city.id = cfg.id;
    city.original_poisson_disks = create_poisson_disks(cfg, max_building_sz, random);
    auto poisson_disks = remove_obstacle_overlaps(city.original_poisson_disks, cfg);

    auto points = poisson_disks | std::views::transform([&](geo::Shape const& disk) { return disk.center(); });
    auto closest_points = geo::Point::closest_points({ points.begin(), points.end() }, cfg.center, cfg.buildings.size());

    auto circles = closest_points | std::views::transform([&](geo::Point const& p) { return geo::Shape::Circle(p, max_building_sz); });
    city.poisson_disks = { circles.begin(), circles.end() };

    city.buildings = create_buildings(cfg.buildings, city.poisson_disks, cfg.city_direction, cfg.angle_variation, random);

    std::vector<geo::Point> bpoints;
    for (auto const& building: city.buildings)
        bpoints.insert(bpoints.end(),
                std::get<geo::shape::Polygon>(building.shape.for_visit()).cbegin(),
                std::get<geo::shape::Polygon>(building.shape.for_visit()).cend());
    city.boundary = geo::Point::convex_hull(bpoints).expand(cfg.boundary_size);

    return city;
}

}