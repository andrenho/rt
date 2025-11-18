#include "map.hh"

#include <format>
#include <string>
#include <fstream>
#include <optional>

#define SVG_W 1000
#define SVG_H 1000

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

using namespace map;

static std::string svg_shape(Map const& map, Shape const& shape, std::string const& color, std::optional<std::string> const& stroke={})
{
    std::string tag = std::visit(overloaded {
        [&](Polygon const& polygon) {
            std::string s = "  <polygon points=\"";
            for (auto const& p: polygon)
                s += std::format("{}, {} ", p.x, p.y);
            return s + "\"";
        },
        [&](Circle const& circle) {
            return std::format(R"(  <circle cx="{}" cy="{}" r="{}")",
                circle.center.x, circle.center.y, circle.radius);
        }
    }, shape);

    tag += " fill=\"" + color + "\"";
    if (stroke)
        tag += " stroke=\"" + *stroke + "\" stroke-width=\"60\"";

    return tag + " />";
}

static std::string terrain_color(Terrain::Type terrain_type)
{
    switch (terrain_type) {
        case Terrain::Type::Dirt:  return "#ffff00";
        default: return "#000000";
    }
}

static void generate_simple_map(map::Map const& map)
{
    std::ofstream f;
    f.open("simple.svg");
    if (!f.is_open())
        return;

    f << std::format("<svg width=\"{}\" height=\"{}\" viewBox=\"{} {} {} {}\" xmlns=\"http://www.w3.org/2000/svg\">\n",
        SVG_W, SVG_H, map.bounds().top_left.x, map.bounds().top_left.y,
        map.bounds().bottom_right.x - map.bounds().top_left.x, map.bounds().bottom_right.y - map.bounds().top_left.y);

    // terrains
    for (auto const& terrain: map.terrains())
        f << svg_shape(map, terrain.shape, terrain_color(terrain.type), "black");

    f << "</svg>";

    f.close();
    std::puts("Generated simple.svg.");
}

int main()
{
    MapConfig cfg {};
    Map map(cfg);

    generate_simple_map(map);
}