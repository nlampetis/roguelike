#include <gtest/gtest.h>
#include <SDL3/SDL_pixels.h>

#include "../src/utils.h"

TEST(general, sanity) {
    EXPECT_STRNE("ok", "nok");
    EXPECT_STREQ("ok", "ok");
}

TEST(utils , color_from_hex) {
    EXPECT_EQ(
        get_sdl_color_from_hex(0x46c864ff).r ,
        70
    );
    EXPECT_EQ(
        get_sdl_color_from_hex(0x46c864ff).g ,
        200
    );
    EXPECT_EQ(
        get_sdl_color_from_hex(0x46c864ff).b ,
        100
    );
    EXPECT_EQ(
        get_sdl_color_from_hex(0x46c864ff).a ,
        255
    );
}

TEST(utils , color_to_hex) {
    constexpr SDL_Color color = {70, 200, 100, 255};
    EXPECT_EQ(
        get_hex_color_from_sdl_color(color) ,
        0x46c864ff
    );
}
