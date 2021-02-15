cmake_minimum_required(VERSION 3.10)

project(Net-Game C)

set(CMAKE_C_STANDARD 11)

option(ENABLE_DEBUG "Debug options" ON)


if(ENABLE_DEBUG)
    set(CMAKE_C_FLAGS "-std=c17 -g --coverage -Wall -Wextra")
    set(CMAKE_LD_FLAGS "--coverage")
    include(CTest)
else()
    set(CMAKE_C_FLAGS "-std=c17 -O2 -Wall -Wextra")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/sdl2-cmake-scripts")

set(NET_HEADERS_DIR "${CMAKE_SOURCE_DIR}/src/include")

add_subdirectory(src)
add_subdirectory(test)