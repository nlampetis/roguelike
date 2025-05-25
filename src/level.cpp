#include "level.h"

#include <spdlog/spdlog.h>

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
    std::vector<std::vector<Tile>> map (_height, std::vector<Tile>(_width));

    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            Tile current_tile {i, j};
            current_tile.set_position(j, i);
            current_tile.set_type(Sky);
            current_tile.set_passable(true);
            map[i][j] = current_tile;
        }
    }

    // std::random_device rd;
    // std::mt19937 gen(rd());

    for (const auto &[x, y, w, h]: rects) {

        // std::uniform_int_distribution wall_for_door(0, 3);
        // if (wall_for_door(gen) )
        // std::



        for (int i = x; i < x + w; ++i) {
            for (int j = y; j < y + h; ++j) {
                Tile t{i, j};
                if (i == x | i == x + w - 1 | j == y | j == y + h - 1) {
                    const int middle_point = x + w/2;
                    if (j == y + h -1 && i == middle_point) {
                        t.set_type(Plains);
                        t.set_passable(true);
                    }
                    else {
                        t.set_type(Mountain);
                        t.set_passable(false);
                    }
                } else {
                    t.set_type(Plains);
                    t.set_passable(true);
                }
                map[j][i] = t;
            }
        }
    }

    _tiles = map;
}

void level::update(const uint64_t delta, const GameEvent event) const {
    p->blink(delta);
    player_act(event);
}

void level::player_act(const GameEvent event) const {


    const auto [x, y] = p->get_destination_coords();
    switch (event) {
        case MOVE_UP:
            if (y-1 >= 0 && _tiles[y-1][x].is_passable()) {
                p->move(UP);
            }
            break;
        case MOVE_DOWN:
            if (y+1 < _height && _tiles[y+1][x].is_passable()) {
                p->move(DOWN);
            }
            break;
        case MOVE_LEFT:
            if (x-1 >= 0 && _tiles[y][x - 1].is_passable()) {
                p->move(LEFT);
            }
            break;
        case MOVE_RIGHT:
            if (x+1 < _width && _tiles[y][x + 1].is_passable()) {
                p->move(RIGHT);
            }
            break;
        default:
            break;
    }
}

Player *level::get_player() const {
    return p;
}


std::vector<std::vector<Tile> > level::get_tilemap() {
    return _tiles;
}

