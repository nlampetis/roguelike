#pragma once


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
    const uint8_t a = (color_hex & 0x000000ff);
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