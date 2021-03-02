add_executable(net_text net_text.c draw_game.c)
target_link_libraries(net_text PRIVATE project_warnings project_options ${GAME_LIBS} rand)

add_executable(net_display net_display.c draw_game.c)
target_link_libraries(net_display PRIVATE project_warnings project_options ${GAME_LIBS})
