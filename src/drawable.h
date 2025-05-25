#pragma once

#include "utils.h"

class Drawable {
public:
    virtual ~Drawable() = default;

    [[nodiscard]] virtual TextureSheet texture_type() const = 0;
    [[nodiscard]] virtual Vector2D_Int get_texture_coords() const = 0;
    [[nodiscard]] virtual Vector2D_Int get_destination_coords() const = 0;
    [[nodiscard]] virtual bool is_hidden() const = 0;
};
