#ifndef __BOOL_ARRAY_H__
#define __BOOL_ARRAY_H__
#include <stdbool.h>
#include <stdlib.h>

/**
 * @file bool_array.h
 *
 * @brief This file provides functions to allocate, check and free 2D boolean
 *arrays.
 **/

bool** alloc_double_bool_array(size_t nb_lines, size_t line_length);

void free_double_bool_array(bool** double_array, size_t nb_lines);

bool check_double_bool_array(bool** double_array, size_t nb_lines,
                             size_t line_length);

#endif  // __BOOL_ARRAY_H__
