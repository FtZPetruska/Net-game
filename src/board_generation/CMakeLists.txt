add_executable(net_gen net_gen.c)
target_link_libraries(net_gen PRIVATE project_warnings project_options ${GAME_LIBS} rand)
