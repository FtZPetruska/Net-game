include_directories(${NET_HEADERS_DIR})

add_executable(tests_game tests_game.c)
target_link_libraries(tests_game PRIVATE project_warnings project_options game)

add_test(new_game               tests_game   new_game)
add_test(game_height            tests_game   game_height)
add_test(rotate_piece           tests_game   rotate_piece)
add_test(is_edge                tests_game   is_edge)
add_test(get_piece              tests_game   get_piece)
add_test(is_game_over           tests_game   is_game_over)
add_test(set_piece              tests_game   set_piece)
add_test(game_width             tests_game   game_width)
add_test(set_piece_current_dir  tests_game   set_piece_current_dir)
add_test(opposite_dir           tests_game   opposite_dir)
add_test(delete_game            tests_game   delete_game)
add_test(restart_game           tests_game   restart_game)
add_test(new_game_empty_ext     tests_game   new_game_empty_ext)
add_test(new_game_ext           tests_game   new_game_ext)
add_test(new_game_empty         tests_game   new_game_empty)
add_test(shuffle_dir            tests_game   shuffle_dir)
add_test(rotate_piece_one       tests_game   rotate_piece_one)
add_test(is_edge_coordinates    tests_game   is_edge_coordinates)
add_test(copy_game              tests_game   copy_game)
add_test(get_current_dir        tests_game   get_current_dir)
add_test(is_wrapping            tests_game   is_wrapping)

add_executable(tests_bool_array tests_bool_array.c)
target_link_libraries(tests_bool_array PRIVATE bool_array project_warnings project_options)

add_test(alloc_double_bool_array            tests_bool_array    alloc_double_bool_array)

add_test(free_double_bool_array_valid       tests_bool_array    free_double_bool_array_valid)
add_test(free_double_bool_array_null_array  tests_bool_array    free_double_bool_array_null_array)
add_test(free_double_bool_array_null_line   tests_bool_array    free_double_bool_array_null_line)

add_test(check_double_bool_array_true       tests_bool_array    check_double_bool_array_true)
add_test(check_double_bool_array_false      tests_bool_array    check_double_bool_array_false)
add_test(check_double_bool_array_null_array tests_bool_array    check_double_bool_array_null_array)
add_test(check_double_bool_array_null_line  tests_bool_array    check_double_bool_array_null_line)


