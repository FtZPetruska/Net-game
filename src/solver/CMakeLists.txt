cmake_minimum_required(VERSION 3.10)

add_executable( net_solve 
    net_solve.c
    solve_smart.c
)
target_link_libraries(net_solve game)

add_executable(net_solve_brute 
    net_solve.c 
    solve_brut.c
)
target_link_libraries(net_solve_brute game)