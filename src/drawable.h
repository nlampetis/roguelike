#pragma once
#include <SDL3/SDL_render.h>

#include "utils.h"

class Drawable {
public:
    virtual ~Drawable() = default;

    [[nodiscard]] virtual SDL_Texture * get_texture() const = 0;
    [[nodiscard]] virtual Vector2D_Int get_texture_coords() const = 0;
    [[nodiscard]] virtual Vector2D_Int get_destination_coords() const = 0;
    [[nodiscard]] virtual bool is_hidden() const = 0;
};
