#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

#include "level.h"
#include "player.h"
#include "tile.h"

// sdl related globals
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

// ttf globals
TTF_Font * font = nullptr;
TTF_TextEngine * text_engine = nullptr;

// tile dimensions
constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;

// window dimensions in tiles
constexpr int TILES_WINDOW_WIDTH = 100;
constexpr int TILES_WINDOW_HEIGHT = 60;

// window dimensions
constexpr int WINDOW_WIDTH = TILE_WIDTH * TILES_WINDOW_WIDTH;
constexpr int WINDOW_HEIGHT = TILE_HEIGHT * TILES_WINDOW_HEIGHT;

// title
constexpr std::string WINDOW_TITLE = "Roguelike";


SDL_Texture * player_texture;
SDL_Texture * lands_tileset_texture;
// rng
auto seed = std::random_device()();
std::default_random_engine generator (seed);
std::uniform_int_distribution distribution(0,3);

bool init() {
    // init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to initialise SDL with error: %s",
            SDL_GetError()
        );
        return false;
    }

    // window and renderer
    SDL_CreateWindowAndRenderer(
        WINDOW_TITLE.c_str(),
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0,
        &window,
        &renderer
    );


    if (!SDL_SetRenderVSync(renderer, 1)) {
        SDL_Log("Failed to enable VSync with error: %s", SDL_GetError());
    }

    if (renderer == nullptr || window == nullptr) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to create window and renderer with error: %s",
            SDL_GetError()
        );
        return false;
    }


    if (!TTF_Init()) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to initialize font library with error: %s",
            SDL_GetError()
        );
        return false;
    }

    font = TTF_OpenFont("../assets/fonts/BigBlueTerm437NerdFont-Regular.ttf", 32);
    if (font == nullptr) {
        SDL_Log("Failed to open font with error: %s", SDL_GetError());
    }

    text_engine = TTF_CreateRendererTextEngine(renderer);
    if (text_engine == nullptr) {
        SDL_Log("Failed to create text engine with error: %s", SDL_GetError());
    }

    return true;
}


void cleanup() {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    TTF_Quit();
    SDL_Quit();
}



std::unordered_map<SDL_Keycode, GameEvent> keymap =
{
    {SDLK_K, MOVE_UP},
    {SDLK_J, MOVE_DOWN},
    {SDLK_H, MOVE_LEFT},
    {SDLK_L, MOVE_RIGHT},
    {SDLK_Q, QUIT},

};

GameEvent handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                return QUIT;
            case SDL_EVENT_KEY_DOWN:
                if (keymap.contains(event.key.key)) {
                    return keymap.at(event.key.key);
                }
                break;
            default:
                break;
        }
    }
    return NONE;
}


SDL_Texture * load_texture(const std::string & path) {
    SDL_Surface *surface = IMG_LoadPNG_IO(SDL_IOFromFile(path.c_str(), "r"));
    if (surface == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load image with error: %s", SDL_GetError());
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (tex == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load image with error: %s", SDL_GetError());
    }

    SDL_DestroySurface(surface);
    return tex;
}


void draw_message(const std::string & message, const SDL_Color color, const float x, const float y) {

    TTF_Text * text = TTF_CreateText(text_engine, font, message.c_str(), message.length());

    if (text == nullptr) {
        SDL_Log("Failed to create text with error: %s", SDL_GetError());
    }

    if (!TTF_SetTextColor(text, color.r, color.g, color.b, color.a)) {
        SDL_Log("Failed to set text color with error: %s", SDL_GetError());
    }
    TTF_DrawRendererText(text, x, y);
}


void draw_fps(const uint64_t delta) {
    const float fps = 1000 / static_cast<float>(delta);
    draw_message(
            std::to_string( static_cast<int>(std::floor(fps))),
            {0,0,0,255},
            WINDOW_WIDTH - 10 - 32*3,
            50
        );
}


// currently only implemented for the case where the display tile dimensions
// are the same as the tileset tile dimensions
void draw(const Drawable &drawable) {

    SDL_Texture * current_tex = nullptr;
    switch (drawable.texture_type()) {
        case PLAYER:
            current_tex = player_texture;
            break;
        case TERRAIN:
            current_tex = lands_tileset_texture;
            break;
    }
    const SDL_Rect source_rect  {
        drawable.get_texture_coords().x * TILE_WIDTH,
        drawable.get_texture_coords().y * TILE_HEIGHT,
        TILE_WIDTH,
        TILE_HEIGHT
    };

    const SDL_Rect dest_rect  {
        drawable.get_destination_coords().x * TILE_WIDTH,
        drawable.get_destination_coords().y * TILE_HEIGHT,
        TILE_WIDTH,
        TILE_HEIGHT
    };

    SDL_SetTextureAlphaMod(current_tex, drawable.is_hidden() ? 0 : 255);


    SDL_FRect source_f_rect;
    SDL_FRect dest_f_rect;

    SDL_RectToFRect(&source_rect, &source_f_rect);
    SDL_RectToFRect(&dest_rect, &dest_f_rect);
    SDL_RenderTexture(renderer, current_tex, &source_f_rect, &dest_f_rect);
}


int main() {

    // initialize SDL
    if (!init()) {
        exit(-1);
    }

    // load textures
    player_texture = load_texture("../assets/images/player_symbol_tiled.png");
    lands_tileset_texture = load_texture("../assets/images/lands_32.png");

    // create the player
    Player p {32, 32};

    level lv {TILES_WINDOW_WIDTH, TILES_WINDOW_HEIGHT, &p};
    lv.generate_level();

    // frame rate and quit variables
    uint64_t previous_ticks = SDL_GetTicks();
    bool quit = false;


    // main loop start
    while (!quit) {

        //frame rate and delta
        const uint64_t now = SDL_GetTicks();
        const uint64_t delta = now - previous_ticks;
        previous_ticks = now;


        // clear the screen before drawing anything
        SDL_SetRenderDrawColor(renderer, 1, 2, 3, 255);
        SDL_RenderClear(renderer);


        // event handling
        const auto current_event = handle_events();
        if (current_event == QUIT) quit = true;


        lv.update(delta, current_event);


        // render all
        for (const auto t : lv.get_tilemap()) {
            for (const auto &t1 : t) {
                draw(t1);
            }
        }
        draw(*lv.get_player());

        // present to the screen
        SDL_RenderPresent(renderer);
    }

    cleanup();
    return 0;
}
