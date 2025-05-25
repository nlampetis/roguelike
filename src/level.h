#pragma once

#include <vector>

#include "player.h"
#include "tile.h"

class level {

    const int _width, _height;

    std::vector<Tile> _tiles;
    Player * p;

public:
    level (int width, int height, Player * p);
    ~level();

    void generate_level();
    void update(uint64_t delta) const;
    std::vector<Tile> get_tilemap();

};

