cmake_minimum_required(VERSION 3.31)
project(trabalho_allegro)
# Include Allegro headers from your project folder
include_directories(${CMAKE_SOURCE_DIR}/allegro/include)
# Add the Allegro library path
link_directories(${CMAKE_SOURCE_DIR}/allegro/lib)
set(CMAKE_CXX_STANDARD 26)

add_executable(trabalho_allegro main.c)

# Link Allegro libraries — add/remove components as needed
target_link_libraries(trabalho_allegro
        allegro
        allegro_image
        allegro_font
        allegro_ttf
        allegro_primitives
)