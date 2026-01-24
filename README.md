# rt

**rt** (road trader) is Elite-inspired game where, in a post-apocalyptic world, you drive through dangerous roads
trading goods between isolated towns. As you grow your riches, new special mission become available, and you need to
choose between becoming a trade lord, a pirate or an avenger.

## Implementation

The game is implemented in C++ using `raylib`, `box2d` and various other geometry libraries. It's made of the following
modules, each present in one toplevel directory. Each module has a sample executable to test the module features, and
can be built independently.

| Module  | Description                                                                   |
|---------|-------------------------------------------------------------------------------|
| common  | Code common to all modules, such as geometry and random number generation     |
| topdown | High level physics engine, with people, cars and explosions                   |
| map     | World map generation                                                          |
| game    | Game engine                                                                   |
| ui      | Game UI                                                                       |

## Building

```shell
mkdir
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
