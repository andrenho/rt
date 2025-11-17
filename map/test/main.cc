#include "map.hh"

#include <string>
#include <fstream>
#include <iostream>

static void generate_simple_map(map::Map const& map)
{
    std::ofstream f;
    f.open("simple.svg");
    if (!f.is_open())
        return;

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
    map::Map map;

    generate_simple_map(map);
    generate_detailed_map(map);

    std::cout << "Generated files `simple.svg` and `detailed.svg`.\n";
}