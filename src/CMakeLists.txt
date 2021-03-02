option(ENABLE_NET_TEXT "Enable compilation of the terminal-based interface" ON)
option(ENABLE_SOLVER "Enable compilation of the solver" OFF)
option(ENABLE_NET_SDL "Enable compilation of the SDL2-based interface" OFF)
option(ENABLE_GENERATOR "Enable compilation of a board generator" OFF)

include_directories(include)

set(GAME_SOURCE_FILES game.c game_io.c cell.c)
add_library(game STATIC ${GAME_SOURCE_FILES})
target_link_libraries(game project_options project_warnings)

set(RAND_SOURCE_FILES game_rand.c)
add_library(rand STATIC ${RAND_SOURCE_FILES})
target_link_libraries(rand project_options project_warnings)

add_library(bool_array bool_array.c)
target_link_libraries(bool_array project_options project_warnings)

set(GAME_LIBS game bool_array)
set(GAME_LIBS ${GAME_LIBS} PARENT_SCOPE)

if(ENABLE_NET_TEXT)
    add_subdirectory(terminal)
endif()

if(ENABLE_SOLVER)
    add_subdirectory(solver)
endif()

if(ENABLE_NET_SDL)
    add_subdirectory(sdl2)
endif()

if(ENABLE_GENERATOR)
    add_subdirectory(board_generation)
endif()