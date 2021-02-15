cmake_minimum_required(VERSION 3.10)

include_directories(include)

set(GAME_SOURCE_FILES
    game.c
    game_io.c
)
add_library(game STATIC ${GAME_SOURCE_FILES})

set(RAND_SOURCE_FILES
    game_rand.c
)
add_library(rand STATIC ${RAND_SOURCE_FILES})


add_subdirectory(terminal)
add_subdirectory(solver)
add_subdirectory(sdl2)
add_subdirectory(board_generation)