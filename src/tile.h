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
    TileType _type;
    int _x, _y;
    bool _hidden;
    bool _passable;
public:
    Tile();
    Tile(int x, int y);
    ~Tile() override;

    [[nodiscard]] TextureSheet texture_type() const override;

    [[nodiscard]] Vector2D_Int get_texture_coords() const override;

    [[nodiscard]] Vector2D_Int get_destination_coords() const override;

    [[nodiscard]] bool is_hidden() const override;

    [[nodiscard]] bool is_passable() const;

    void set_passable(bool passable);

    void set_position(int x, int y);

    void set_type(TileType t);
};
