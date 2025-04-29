cmake_minimum_required(VERSION 3.31)
project(roguelike)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
include_directories(roguelike ${SDL3_INCLUDE_DIRS})
include_directories(roguelike ${SDL3_IMAGE_INCLUDE_DIRS})
include_directories(roguelike ${SDL3_TTF_INCLUDE_DIRS})

add_executable(roguelike main.cpp
        player.cpp
        player.h
        drawable.h
        game_object.h
        tile.cpp
        tile.h
        utils.h)
target_link_libraries(roguelike ${SDL3_LIBRARIES})
target_link_libraries(roguelike SDL3_image::SDL3_image)
target_link_libraries(roguelike SDL3_ttf)

