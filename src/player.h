#pragma once

#include <SDL3/SDL_render.h>

#include "drawable.h"
#include "game_object.h"
#include "utils.h"


class Player final : public GameObject, public Drawable {

    SDL_Texture * _texture = nullptr;
    int _tile_pos_x;
    int _tile_pos_y;
    uint64_t _blink_timer = 0;
    bool _hidden = false;

public:

    Player(int x, int y);
    ~Player() override;

    void set_texture(SDL_Texture * texture);

    void move(Direction d);

    void blink(uint64_t delta);

    void toggle_hidden();

    void hide();

    void unhide();

    [[nodiscard]] SDL_Texture * get_texture() const override;

    [[nodiscard]] Vector2D_Int get_destination_coords() const override;

    [[nodiscard]] Vector2D_Int get_texture_coords() const override;

    [[nodiscard]] bool is_hidden() const override;
};
