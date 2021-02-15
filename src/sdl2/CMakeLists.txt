cmake_minimum_required(VERSION 3.10)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

set( SDL2_ALL_INC
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_MIXER_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
)

set( SDL2_ALL_LIBS 
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

include_directories(${SDL2_ALL_INC})

add_executable( net_sdl
    net_graphic.c
    sdl_graphic.c
    ../solver/solve_smart.c
)

target_link_libraries( net_sdl
    ${SDL2_ALL_LIBS}
    m
    game
    rand
)

file(COPY assets DESTINATION assets)