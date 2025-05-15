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


bool handle_events(Player &player) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                return true;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_Q:
                        return true;
                    case SDLK_W:
                        player.move(UP);
                        break;
                    case SDLK_A:
                        player.move(LEFT);
                        break;
                    case SDLK_S:
                        player.move(DOWN);
                        break;
                    case SDLK_D:
                        player.move(RIGHT);
                        break;
                    default: break;
                }
            default:
                break;
        }
    }
    return false;
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

    SDL_SetTextureAlphaMod(drawable.get_texture(), drawable.is_hidden() ? 0 : 255);


    SDL_FRect source_f_rect;
    SDL_FRect dest_f_rect;

    SDL_RectToFRect(&source_rect, &source_f_rect);
    SDL_RectToFRect(&dest_rect, &dest_f_rect);
    SDL_RenderTexture(renderer, drawable.get_texture(), &source_f_rect, &dest_f_rect);
}


std::vector<TileRect> generate_space_partition_map_tiles(TileRect tr) {
    constexpr auto split_threshold_w = 15;
    constexpr auto split_threshold_h = 15;

    constexpr auto min_w = split_threshold_w /2;
    constexpr auto min_h = split_threshold_h /2;

    if (tr.h < min_h | tr.w < min_w) {
        return {};
    }

    if (tr.h <= split_threshold_h & tr.w <= split_threshold_w) {
        return {tr};
    }


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution how_to_split (0, 1);

    const bool is_vertical_split = how_to_split(gen);
    // if (tr.h <= split_threshold_h)

    std::vector<TileRect> a {};
    std::vector<TileRect> b {};

    if (is_vertical_split) {

        std::uniform_int_distribution where_to_split (tr.w/4, 3*tr.w/4);
        const auto split_point = where_to_split(gen);

        a = generate_space_partition_map_tiles( {tr.x, tr.y, split_point, tr.h} );
        b = generate_space_partition_map_tiles({tr.x + split_point, tr.y, tr.w - split_point, tr.h});
    } else {

        std::uniform_int_distribution where_to_split (tr.h/4, 3*tr.h/4);
        const auto split_point = where_to_split(gen);

        a = generate_space_partition_map_tiles({tr.x, tr.y, tr.w, split_point});
        b = generate_space_partition_map_tiles({tr.x, tr.y + split_point, tr.w, tr.h - split_point});
    }

    a.insert(a.end(), b.begin(), b.end());
    return a;
}

std::vector<Tile> generate_normal_distribution_map(SDL_Texture * lands_tileset_texture) {

    // generate the map
    std::vector<Tile> map;
    for (int i = 0; i < TILES_WINDOW_WIDTH; ++i) {
        for (int j = 0; j < TILES_WINDOW_HEIGHT; ++j) {
            const int dice_roll = distribution(generator);
            Tile t {i, j};
            t.set_texture(lands_tileset_texture);
            switch (dice_roll) {
                case 0:
                    t.set_type(Mountain);
                    break;
                case 1:
                    t.set_type(Sky);
                    break;
                case 2:
                    t.set_type(Plains);
                    break;
                case 3:
                    t.set_type(Forest);
                    break;
                default:
                    break;

            }
            map.push_back(t);
        }
    }

    return map;
}


int main() {

    // initialize SDL
    if (!init()) {
        exit(-1);
    }

    // load textures
    SDL_Texture *player_texture = load_texture("../assets/images/player_symbol_tiled.png");
    SDL_Texture * lands_tileset_texture = load_texture("../assets/images/lands_32.png");

    // create the player
    Player p {32, 32};
    p.set_texture(player_texture);


    // frame rate and quit variables
    uint64_t previous_ticks = SDL_GetTicks();
    bool quit = false;


    constexpr TileRect tr {0, 0, TILES_WINDOW_WIDTH, TILES_WINDOW_HEIGHT};

    std::vector<TileRect> rects = generate_space_partition_map_tiles(tr);
    constexpr auto padding = 1;
    for (auto &[x, y, w, h] : rects) {
        x += padding;
        y += padding;
        w -= 2 * padding;
        h -= 2 * padding;
    }


    std::array<Tile, TILES_WINDOW_HEIGHT * TILES_WINDOW_WIDTH> map = {};

    for (const auto &[x, y, w, h] : rects ) {
        for (int i = x; i < x + w; ++i) {
            for (int j = y; j < y + h; ++j) {
                Tile t {i, j};
                t.set_texture(lands_tileset_texture);
                if (i == x | i == x + w - 1 | j == y | j == y + h - 1) {
                    t.set_type(Mountain);
                    t.set_passable(false);
                } else {
                    t.set_type(Plains);
                    t.set_passable(true);
                }
                map.at(i*TILES_WINDOW_HEIGHT+j) = t;
            }
        }
    }

    for (int i = 0; i < TILES_WINDOW_HEIGHT; ++i) {
        for (int j = 0; j < TILES_WINDOW_WIDTH; ++j) {
            auto & current_tile = map.at(j * TILES_WINDOW_HEIGHT + i);
            if (current_tile.get_texture() == nullptr) {
                current_tile.set_texture(lands_tileset_texture);
                current_tile.set_position(j, i);
                current_tile.set_type(Sky);
                current_tile.set_passable(true);
            }
        }
    }


    // main loop start
    while (!quit) {

        //frame rate and delta
        const uint64_t now = SDL_GetTicks();
        const uint64_t delta = now - previous_ticks;
        previous_ticks = now;


        // clear the screen before drawing anything
        SDL_SetRenderDrawColor(renderer, 1, 2, 3, 255);
        SDL_RenderClear(renderer);

        for (const auto &t : map) {
            draw(t);
        }

        // event handling
        quit = handle_events(p);

         // game logic
         p.blink(delta);


         // render player
         draw(p);

         // draw fps
         // draw_fps(delta);

        // present to the screen
        SDL_RenderPresent(renderer);
    }

    cleanup();
    return 0;
}
