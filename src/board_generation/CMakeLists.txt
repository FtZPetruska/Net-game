cmake_minimum_required(VERSION 3.10)

add_executable(net_gen net_gen.c)
target_link_libraries( net_gen
    game
    rand
)