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

if(WIN32)
    set(MATH_LIB)
else()
    set(MATH_LIB m)
endif()

include_directories(${SDL2_ALL_INC})

add_executable(net_sdl net_graphic.c sdl_graphic.c ../solver/solve_smart.c)
target_link_libraries(net_sdl PRIVATE project_warnings project_options ${SDL2_ALL_LIBS} ${MATH_LIB} ${GAME_LIBS} rand)

file(COPY assets DESTINATION .)
