#pragma once

#include <SDL3/SDL_render.h>

#include "drawable.h"
#include "utils.h"


class Player final : public Drawable {

    int _tile_pos_x;
    int _tile_pos_y;
    uint64_t _blink_timer = 0;
    bool _hidden = false;

public:

    Player(int x, int y);
    ~Player() override;

    void move(Direction d);

    void blink(uint64_t delta);

    void toggle_hidden();

    void hide();

    void unhide();

    [[nodiscard]] Vector2D_Int get_destination_coords() const override;

    [[nodiscard]] Vector2D_Int get_texture_coords() const override;

    [[nodiscard]] bool is_hidden() const override;

    [[nodiscard]] TextureSheet texture_type() const override;
};
