cmake_minimum_required(VERSION 3.31)
project(space_invaders)
# Include Allegro headers from your project folder
include_directories(${CMAKE_SOURCE_DIR}/allegro/include)
# Add the Allegro library path
link_directories(${CMAKE_SOURCE_DIR}/allegro/lib)
set(CMAKE_CXX_STANDARD 26)

add_executable(space_invaders main.c
        game_state.h
        game_state.c
        menu_state.h
        stdafx.h
        menu_state.c
        gameover_state.c
        gameover_state.h
)

# Link Allegro libraries — add/remove components as needed
target_link_libraries(space_invaders
        allegro
        allegro_image
        allegro_font
        allegro_ttf
        allegro_primitives
        allegro_audio
        allegro_acodec
)