#pragma once
#include "drawable.h"

enum TileType {
    Mountain,
    Sky,
    Forest,
    Plains
};

Vector2D_Int map_type_to_texture_coords(TileType);


class Tile final : public Drawable {
    SDL_Texture * _tex;
    TileType _type;
    int _x, _y;
    bool _hidden;
    bool _passable;
public:
    Tile(int x, int y);
    ~Tile() override;

    [[nodiscard]] SDL_Texture * get_texture() const override;

    [[nodiscard]] Vector2D_Int get_texture_coords() const override;

    [[nodiscard]] Vector2D_Int get_destination_coords() const override;

    [[nodiscard]] bool is_hidden() const override;

    [[nodiscard]] bool is_passable() const;

    void set_texture(SDL_Texture * tex);

    void set_type(TileType t);
};
