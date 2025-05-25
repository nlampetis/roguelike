#pragma once

#include <vector>

#include "player.h"
#include "tile.h"

enum GameEvent {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    QUIT,
    NONE
};

class level {

    const int _width, _height;
    std::vector<std::vector<Tile>> _tiles;
    Player * p;

public:
    level (int width, int height, Player * p);
    ~level();

    void generate_level();
    void update(uint64_t delta, GameEvent event) const;

    void player_act(GameEvent event) const;

    Player * get_player() const;
    std::vector<std::vector<Tile>> get_tilemap();

};

