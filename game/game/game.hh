#ifndef RT_GAME_HH
#define RT_GAME_HH

#include "fastecs.hh"
#include "components.hh"

namespace game {

struct GameConfig {};

using Game = ecs::ECS<ecs::NoGlobal, ecs::NoMessageQueue, ecs::NoPool, COMPONENTS>;

Game game_init(GameConfig const& cfg);

}

#endif //RT_GAME_HH
