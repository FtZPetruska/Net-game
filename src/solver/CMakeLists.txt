add_executable(net_solve net_solve.c solve_smart.c)
target_link_libraries(net_solve PRIVATE project_warnings project_options game)

add_executable(net_solve_brute net_solve.c solve_brut.c)
target_link_libraries(net_solve_brute PRIVATE project_warnings project_options game)
