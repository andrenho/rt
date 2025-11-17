#include "map.hh"

#include <format>
#include <string>
#include <fstream>
#include <iostream>

#define SVG_W 1000
#define SVG_H 1000

using namespace map;

static void generate_simple_map(map::Map const& map)
{
    std::ofstream f;
    f.open("simple.svg");
    if (!f.is_open())
        return;

    auto px = [&map](float x) { return SVG_W / (map.bounds().bottom_right.x - map.bounds().top_left.x) * x; };
    auto py = [&map](float y) { return SVG_H / (map.bounds().bottom_right.y - map.bounds().top_left.y) * y; };

    f << std::format("<svg width=\"{}\" height=\"{}\" xmlns=\"http://www.w3.org/2000/svg\">\n", SVG_W, SVG_H);
    f << "</svg>";

    f.close();
}

static void generate_detailed_map(map::Map const& map)
{
    std::ofstream f;
    f.open("detailed.svg");
    if (!f.is_open())
        return;

    f.close();
}

int main()
{
    MapConfig cfg {};
    Map map(cfg);

    generate_simple_map(map);
    generate_detailed_map(map);

    std::cout << "Generated files `simple.svg` and `detailed.svg`.\n";
}