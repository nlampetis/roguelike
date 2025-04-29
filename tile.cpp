#include "tile.h"

Tile::Tile(const int x,const int y):
_tex(nullptr), _type(Plains), _x(x), _y(y), _hidden(false), _passable(true) {
}

Tile::~Tile() = default;

SDL_Texture * Tile::get_texture() const {
    return _tex;
}

Vector2D_Int Tile::get_texture_coords() const {
    return map_type_to_texture_coords(_type);
}

Vector2D_Int Tile::get_destination_coords() const {
    return {_x, _y};
}

bool Tile::is_hidden() const {
    return _hidden;
}

bool Tile::is_passable() const {
    return _passable;
}

void Tile::set_texture(SDL_Texture *tex) {
    _tex = tex;
}

void Tile::set_type(const TileType t) {
    _type = t;
}


Vector2D_Int map_type_to_texture_coords(const TileType t) {
    switch (t) {
        case Sky:
            return {0, 0};
        case Mountain:
            return {1,0};
        case Plains:
            return {2,0};
        case Forest:
            return {3,0};
    }
    return {0,0};
}