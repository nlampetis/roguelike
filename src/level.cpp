#include "level.h"

level::level(const int width, const int height, Player *p)
: _width(width), _height(height), p(p) {}

level::~level() {}


void level::generate_level() {
    const TileRect tr{0, 0, _width, _height};
    std::vector<TileRect> rects = generate_bsp_map(tr);


    for (auto &[x, y, w, h]: rects) {
        constexpr auto padding = 1;
        x += padding;
        y += padding;
        w -= 2 * padding;
        h -= 2 * padding;
    }


    const size_t map_size = _height * _width;
    std::vector<Tile> map  {map_size};

    for (int i = 0; i < _height; ++i) {
        for (int j = 0; j < _width; ++j) {
            Tile current_tile {i, j};
            current_tile.set_position(j, i);
            current_tile.set_type(Sky);
            current_tile.set_passable(true);
            map.at(j * _height + i) = current_tile;
        }
    }

    for (const auto &[x, y, w, h]: rects) {
        for (int i = x; i < x + w; ++i) {
            for (int j = y; j < y + h; ++j) {
                Tile t{i, j};
                if (i == x | i == x + w - 1 | j == y | j == y + h - 1) {
                    t.set_type(Mountain);
                    t.set_passable(false);
                } else {
                    t.set_type(Plains);
                    t.set_passable(true);
                }
                map.at(i * _height + j) = t;
            }
        }
    }

    _tiles = map;
}

void level::update(const uint64_t delta) const {
    p->blink(delta);
}

std::vector<Tile> level::get_tilemap() {
    return _tiles;
}

