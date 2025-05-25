#pragma once
#include <random>
#include <vector>
#include <SDL3/SDL_render.h>

struct Vector2D_Float {
    float x;
    float y;
};

struct Vector2D_Int {
    int x;
    int y;
};

struct TileRect {
    int x;
    int y;
    int w;
    int h;
};

enum TextureSheet {
    PLAYER,
    TERRAIN
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

inline SDL_Color get_sdl_color_from_hex(const uint32_t color_hex) {
    const uint8_t r = (color_hex & 0xff000000) >> 24;
    const uint8_t g = (color_hex & 0x00ff0000) >> 16;
    const uint8_t b = (color_hex & 0x0000ff00) >> 8;
    const uint8_t a = color_hex & 0x000000ff;
    return SDL_Color {r, g, b, a};
}


inline uint32_t get_hex_color_from_sdl_color(const SDL_Color color_sdl) {

    uint32_t color_hex = 0x00000000;
    color_hex = color_hex | color_sdl.a;
    color_hex = color_hex | color_sdl.b << 8 ;
    color_hex = color_hex | color_sdl.g << 16;
    color_hex = color_hex | color_sdl.r << 24;
    return color_hex;
}

inline std::vector<TileRect> generate_bsp_map(TileRect tile_rect) {
    constexpr auto split_threshold_w = 15;
    constexpr auto split_threshold_h = 15;

    constexpr auto min_w = split_threshold_w / 2;
    constexpr auto min_h = split_threshold_h / 2;

    if (tile_rect.h < min_h | tile_rect.w < min_w) {
        return {};
    }

    if (tile_rect.h <= split_threshold_h & tile_rect.w <= split_threshold_w) {
        return {tile_rect};
    }


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution how_to_split(0, 1);

    const bool is_vertical_split = how_to_split(gen);
    // if (tr.h <= split_threshold_h)

    std::vector<TileRect> a{};
    std::vector<TileRect> b{};

    if (is_vertical_split) {
        std::uniform_int_distribution where_to_split(tile_rect.w / 4, 3 * tile_rect.w / 4);
        const auto split_point = where_to_split(gen);

        a = generate_bsp_map({tile_rect.x, tile_rect.y, split_point, tile_rect.h});
        b = generate_bsp_map({tile_rect.x + split_point, tile_rect.y, tile_rect.w - split_point, tile_rect.h});
    } else {
        std::uniform_int_distribution where_to_split(tile_rect.h / 4, 3 * tile_rect.h / 4);
        const auto split_point = where_to_split(gen);

        a = generate_bsp_map({tile_rect.x, tile_rect.y, tile_rect.w, split_point});
        b = generate_bsp_map({tile_rect.x, tile_rect.y + split_point, tile_rect.w, tile_rect.h - split_point});
    }

    a.insert(a.end(), b.begin(), b.end());
    return a;
}
