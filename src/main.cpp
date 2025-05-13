#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

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
    draw_message(std::to_string(static_cast<int>(std::floor(fps))), {0,0,0,255}, WINDOW_WIDTH - 10 - 32*3, 50);
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


void render_sub_rects(const SDL_FRect r) {

    const int x = static_cast<int> (r.x);
    const int y = static_cast<int> (r.y);
    const int w = static_cast<int> (r.w);
    const int h = static_cast<int> (r.h);

    const int mult = 7;

    constexpr int min_width_to_draw = WINDOW_WIDTH /  (mult + 1);
    constexpr int min_height_to_draw = WINDOW_HEIGHT / (mult + 1);

    constexpr int min_width_to_subdivide = WINDOW_WIDTH / mult;
    constexpr int min_height_to_subdivide = WINDOW_HEIGHT / mult;


    if (w < min_width_to_draw || h < min_height_to_draw) {
        return;
    }
    if (w < min_width_to_subdivide || h < min_height_to_subdivide ) {
        const auto current_area = static_cast<Uint8>(w) % 255;
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 255);
        SDL_RenderRect(renderer, &r);
        return;
    }

    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution how_to_split (0, 1);

    SDL_FRect left_division;
    SDL_FRect right_division;


    if (how_to_split(gen) == 0) {

        const int lower_bound = w/4;
        const int upper_bound = w - lower_bound;

        std::uniform_int_distribution where_to_split(lower_bound, upper_bound);
        const int split_point = where_to_split(gen);
        // const int split_point = w/2;

        left_division = {
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(split_point),
            static_cast<float>(h)
        };

        right_division = {
            static_cast<float>(x + split_point),
            static_cast<float>(y),
            static_cast<float>(w - split_point),
            static_cast<float>(h)
        };

    } else {
        const int lower_bound = h/4;
        const int upper_bound = h - lower_bound;

        std::uniform_int_distribution where_to_split(lower_bound, upper_bound);
        const int split_point = where_to_split(gen);
        // const int split_point = h/2;

        left_division = {
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(w),
            static_cast<float>(split_point)
        };

        right_division = {
            static_cast<float>(x),
            static_cast<float>(y + split_point),
            static_cast<float>(w),
            static_cast<float>(h - split_point)
        };
    }

    render_sub_rects(left_division);
    render_sub_rects(right_division);

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
        quit = handle_events(p);

         // game logic
         p.blink(delta);


         // render player
         draw(p);

         // draw fps
         draw_fps(delta);

        // present to the screen
        SDL_RenderPresent(renderer);
    }


    cleanup();
    return 0;
}
