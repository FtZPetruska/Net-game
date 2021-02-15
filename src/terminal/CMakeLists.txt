cmake_minimum_required(VERSION 3.10)

add_executable(net_text net_text.c)
target_link_libraries( net_text
    game
    rand
)

add_executable(net_display net_display.c)
target_link_libraries( net_display
    game
)