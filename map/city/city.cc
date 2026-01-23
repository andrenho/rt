#include "city.hh"

#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

namespace city {

static constexpr float DOOR_LENGTH = 1.5f;

static std::vector<geo::Shape> create_poisson_disks(CityConfig const& cfg, float max_building_sz, Random const& random)
{
    auto points = geo::Point::poisson(
            geo::Shape::Box({ cfg.center.x - cfg.max_size/2.f, cfg.center.y - cfg.max_size/2.f }, { cfg.max_size, cfg.max_size }),
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

static std::tuple<std::vector<geo::Shape>, geo::Shape, geo::Point> open_building_shape(geo::shape::Polygon const& box,
        geo::Point const& building_center, BuildingConfig::Entrance const& entrance)
{
    auto lines = geo::shape::polygon_lines(box);
    geo::Shape sensor;

    // wall containing door
    auto door_line = lines.at(0);
    auto door_point = geo::Point(
            door_line.p1.x + entrance.position * (door_line.p2.x - door_line.p1.x),
            door_line.p1.y + entrance.position * (door_line.p2.y - door_line.p1.y)
    );

    // other walls
    std::vector<geo::Shape> walls;
    for (size_t i = 0; i < lines.size(); ++i) {
        auto const& line = lines.at(i);
        auto p1 = line.p1;
        auto p2 = line.p2;
        auto p3 = p1.perpendicular_endpoint(p2, -entrance.wall_width);
        auto p4 = p2.perpendicular_endpoint(p1, entrance.wall_width);

        if (i == 0) {    // door
            auto midpoint1 = (p2-p1).length() * entrance.position - (DOOR_LENGTH / 2.f);
            auto midpoint2 = (p2-p1).length() * entrance.position + (DOOR_LENGTH / 2.f);
            auto pd1 = p1.point_at_distance(p2, midpoint1);
            auto pd2 = p1.point_at_distance(p2, midpoint2);
            auto pd3 = p1.perpendicular_endpoint(pd2, -entrance.wall_width);
            auto pd4 = p1.perpendicular_endpoint(pd1, -entrance.wall_width);
            walls.push_back(geo::Shape::Polygon({ p1, pd1, pd4, p4 }));
            walls.push_back(geo::Shape::Polygon({ pd2, p2, p3, pd3 }));
            sensor = box;
        } else {
            walls.push_back(geo::Shape::Polygon({ p1, p2, p3, p4 }));
        }
    }

    return { walls, sensor, door_point };
}

static City::Building create_building(BuildingConfig const& config, std::variant<float, geo::Point> const& city_direction,
        float angle_variation, Random& random, geo::Point const& disk_center)
{
    // calculate outer box
    float angle = std::visit(overloaded {
            [&](float a) { return a; },
            [&](geo::Point const& center) { return center.angle(disk_center); }
    }, city_direction);

    auto box = geo::Shape::Box(disk_center - geo::Point(config.h / 2.f, config.w / 2.f),
            { config.h, config.w }, angle + random.next_float(-angle_variation, angle_variation));

    std::vector<geo::Shape> walls;
    std::optional<geo::Shape> sensor;
    geo::Point door_position {};

    // entrance
    if (config.entrance) {
        if (config.entrance->position < 0.f || config.entrance->position >= 1.f)
            throw std::runtime_error("'entrance.position' needs to be between 0 and 1");

        std::tie(walls, sensor, door_position) = open_building_shape(std::get<geo::shape::Polygon>(box.for_visit()), box.center(), *config.entrance);
    }

    return {
        .id = config.id,
        .shape = box,
        .walls = walls,
        .sensor = sensor,
        .door_position = door_position,
    };
}

static std::vector<City::Building> create_buildings(std::vector<BuildingConfig> const& buildings,
    std::vector<geo::Shape> const& poisson_disks, std::variant<float, geo::Point> const& city_direction, float angle_variation, Random& random)
{
    if (buildings.size() > poisson_disks.size())
        throw std::runtime_error("Too few poisson disks generated in city building.");

    return std::views::iota(0ULL, std::min(buildings.size(), poisson_disks.size()))
        | std::views::transform([&](size_t i) {
            return create_building(buildings.at(i), city_direction, angle_variation, random, poisson_disks.at(i).center());
        })
        | std::ranges::to<std::vector>();
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