cmake_minimum_required(VERSION 3.31)
project(roguelike)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(spdlog REQUIRED)

include_directories(roguelike ${SDL3_INCLUDE_DIRS})
include_directories(roguelike ${SDL3_IMAGE_INCLUDE_DIRS})
include_directories(roguelike ${SDL3_TTF_INCLUDE_DIRS})

# test related
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)
# test related end

add_executable(roguelike src/main.cpp
        src/player.cpp
        src/player.h
        src/drawable.h
        src/game_object.h
        src/tile.cpp
        src/tile.h
        src/utils.h)

target_link_libraries(roguelike ${SDL3_LIBRARIES})
target_link_libraries(roguelike SDL3_image::SDL3_image)
target_link_libraries(roguelike SDL3_ttf)
target_link_libraries(roguelike spdlog::spdlog)

enable_testing()

add_executable(utils_test tests/utils_tests.cpp)
target_link_libraries(utils_test GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(utils_test)
