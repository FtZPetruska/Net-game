#ifndef __CELL_H__
#define __CELL_H__

#include "game.h"

typedef struct cell_s *cell;

cell alloc_cell();
void free_cell(cell current_cell);
cell get_right_cell(cell current_cell);
cell get_left_cell(cell current_cell);
cell get_top_cell(cell current_cell);
cell get_bottom_cell(cell current_cell);
piece get_piece_cell(cell current_cell);
void set_piece_cell(cell current_cell, piece new_piece);
direction get_current_direction_cell(cell current_cell);
void set_current_direction_cell(cell current_cell,
                                       direction new_direction);
direction get_default_direction_cell(cell current_cell);
void set_default_direction_cell(cell current_cell,
                                       direction new_direction);
bool is_out_of_bounds_cell(cell current_cell);
cell make_out_of_bounds_cell();
cell translate_cell(cell current_cell, int32_t x, int32_t y);
void set_right_cell(cell current_cell, cell target_cell);
void set_left_cell(cell current_cell, cell target_cell);
void set_top_cell(cell current_cell, cell target_cell);
void set_bottom_cell(cell current_cell, cell target_cell);
cell create_cell_line(uint16_t width);
void destroy_cell_line(cell origin);
void destroy_cell_rectangle(cell origin);
void shuffle_direction_cell(cell origin, uint16_t width, uint16_t height);
bool link_lines_cell(cell bottom_origin, cell top_origin);
bool link_columns_cell(cell left_origin, cell right_origin);
bool is_rectangle_valid(cell origin, bool is_wrapped);
void restore_initial_direction_cell(cell current_cell);

#endif // __CELL_H__
