#include "cell.h"

#include <stdio.h>
/**
 * @brief Structure for a board cell
 */
struct cell_s {
  piece cell_piece;                  /**< Piece help by the cell.*/
  direction current_piece_direction; /**< Current direction of the piece.*/
  cell left;   /**< Pointer to the cell to the left, NULL if there is none.*/
  cell right;  /**< Pointer to the cell to the right, NULL if there is none.*/
  cell top;    /**< Pointer to the cell to the top, NULL if there is none.*/
  cell bottom; /**< Pointer to the cell to the bottom, NULL if there is none.*/
  direction default_piece_direction; /**< Default direction of the piece, used
                                        for restart.*/
};

static uint16_t size_column_cell(cell origin) {
  cell current_cell = origin;
  uint16_t count = 0;
  while (!is_out_of_bounds_cell(current_cell) &&
         !(current_cell == origin && count != 0)) {
    count++;
    current_cell = get_top_cell(current_cell);
  }
  return count;
}

static uint16_t line_size_cell(cell origin) {
  cell current_cell = origin;
  uint16_t count = 0;
  while (!is_out_of_bounds_cell(current_cell) &&
         !(current_cell == origin && count != 0)) {
    count++;
    current_cell = get_right_cell(current_cell);
  }
  return count;
}

/**
 * @brief Allocate dynamically a cell, a case of the board
 *
 * @return the pointer to the created cell or NULL in case of error
 **/
cell alloc_cell() {
  cell new_cell = (cell)malloc(sizeof(struct cell_s));
  if (!new_cell) {
    FPRINTF(stderr, "Not enough memory to alloc a cell!\n");
    return make_out_of_bounds_cell();
  }
  // set default values
  new_cell->cell_piece = EMPTY;
  new_cell->current_piece_direction = N;
  new_cell->left = make_out_of_bounds_cell();
  new_cell->right = make_out_of_bounds_cell();
  new_cell->top = make_out_of_bounds_cell();
  new_cell->bottom = make_out_of_bounds_cell();
  new_cell->default_piece_direction = N;
  return new_cell;
}

/**
 * @brief Free a cell
 *
 * @param cell pointer to free
 **/
void free_cell(cell current_cell) {
  if (is_out_of_bounds_cell(current_cell)) {
    FPRINTF(stderr, "Error: free_cell, cannot free NULL pointer.\n");
    return;
  }
  free(current_cell);
}

/**
 * @brief Get pointer to the right of this one
 *
 * @param current_cell, the current cell
 * @return the cell at right of the current cell or NULL in case of error
 **/
cell get_right_cell(cell current_cell) {
  if (current_cell != NULL) {
    return current_cell->right;
  }
  FPRINTF(stderr, "Warning, get_right_cell was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get pointer to the case to the left of this one
 *
 * @param current_cell, the current cell
 * @return the cell at left of the current cell or NULL in case of error
 **/
cell get_left_cell(cell current_cell) {
  if (current_cell != NULL) {
    return current_cell->left;
  }
  FPRINTF(stderr, "Warning, get_left_cell was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get pointer to the above this one
 *
 * @param current_cell, the current cell
 * @return the cell at up of the current cell or NULL in case of error
 **/
cell get_top_cell(cell current_cell) {
  if (current_cell != NULL) {
    return current_cell->top;
  }
  FPRINTF(stderr, "Warning, get_top_cell was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get pointer to the case below this one
 *
 * @param current_cell, the current cell
 * @return the cell at down of the current cell or NULL in case of error
 **/
cell get_bottom_cell(cell current_cell) {
  if (current_cell != NULL) {
    return current_cell->bottom;
  }
  FPRINTF(stderr, "Warning, get_bottom_cell was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get the piece value of the cell
 *
 * @param current_cell, the current cell
 * @return the piece value or -2 in case of error
 **/
piece get_piece_cell(cell current_cell) {
  if (current_cell != NULL) {
    return current_cell->cell_piece;
  }
  FPRINTF(stderr, "Warning, get_piece_cell was used on a NULL pointer\n");
  return (piece)-2;
}

/**
 * @brief Set a cell's piece field
 *
 * @param current_cell, the cell we want to set the piece
 * @param new_piece, the piece we want to set
 **/
void set_piece_cell(cell current_cell, piece new_piece) {
  if (current_cell != NULL)
    current_cell->cell_piece = new_piece;
  else
    FPRINTF(stderr, "Warning, tried to set a piece on a NULL Cell");
}

/**
 * @brief Get the direction value of the cell
 *
 * @param current_cell, the current cell
 * @return the direction value or -1 in case of error
 **/
direction get_current_direction_cell(cell current_cell) {
  if (current_cell != NULL) {
    return current_cell->current_piece_direction;
  }
  FPRINTF(stderr,
          "Warning, get_current_direction_cell was used on a NULL pointer\n");
  return (direction)-1;
}

/**
 * @brief Set a cell's dir field
 *
 * @param current_cell, the cell we want to set the dir
 * @param new_direction, the direction we want to set
 **/
void set_current_direction_cell(cell current_cell, direction new_direction) {
  if (current_cell != NULL)
    current_cell->current_piece_direction = new_direction;
  else
    FPRINTF(stderr, "Warning, tried to set a direction on a NULL Cell");
}

/**
 * @brief Get the default direction value of the cell
 *
 * @param current_cell, the current cell
 * @return the default direction value or N in case of error
 **/
direction get_default_direction_cell(cell current_cell) {
  if (current_cell != NULL) {
    return current_cell->default_piece_direction;
  }
  FPRINTF(stderr, "Warning, getDir was used on a NULL pointer\n");
  return N;
}

/**
 * @brief Set the default direction value of the cell
 *
 * @param current_cell, the current cell
 * @param new_direction, the default direction we want to set
 **/
void set_default_direction_cell(cell current_cell, direction new_direction) {
  if (current_cell != NULL) {
    current_cell->default_piece_direction = new_direction;
  } else {
    FPRINTF(stderr, "Warning, can't set a default direction to a NULL cell\n");
  }
}

/**
 * @brief Check if a cell is out of board's rims.
 *
 * @param current_cell, the cell to test
 * @return true if the cell is not in the game bound.
 **/
bool is_out_of_bounds_cell(cell current_cell) { return current_cell == NULL; }

/**
 * @brief Create a undefined cell
 *
 * @return undefined cell
 **/
cell make_out_of_bounds_cell() { return NULL; }

/**
 * @brief Get the pointer to the cell that is x to the right and y above
 *
 * @param current_cell, the current cell (origin cell in most of case)
 * @param x, how much on the left (x<0) or right (x>0) you want to move
 * @param y, how much down (y<0) or up (y>0) you want to move
 * @return pointer to a cell or NULL in case of error
 **/
cell translate_cell(cell current_cell, int32_t x, int32_t y) {
  // NB: a frequent use is : translate_cell(origin, x , y); which get cell at
  // (x,y) in the board

  if (!current_cell) {
    FPRINTF(stderr, "Warning, trying to translate_cell from a NULL pointer!\n");
    return NULL;
  }

  cell temporary_cell = current_cell;

  while (x != 0) {
    // Tests if the cell we're trying to access is out of bound
    if (!temporary_cell) {
      FPRINTF(stderr, "Error when trying to get cell, x was out of range!\n");
      return NULL;
    }

    if (x < 0) {
      temporary_cell = temporary_cell->left;
      x++;
    }

    else {
      temporary_cell = temporary_cell->right;
      x--;
    }
  }

  while (y != 0) {
    if (!temporary_cell) {
      FPRINTF(stderr, "Error when trying to get cell, y was out of range!\n");
      return NULL;
    }

    if (y < 0) {
      temporary_cell = temporary_cell->bottom;
      y++;
    }

    else {
      temporary_cell = temporary_cell->top;
      y--;
    }
  }

  return temporary_cell;
}

/**
 * @brief Link the cell current_cell to the cell target_cell on the right
 *
 * @param current_cell, the cell which is going to be linked
 * @param target_cell, the cell to which we link current_cell to
 **/
void set_right_cell(cell current_cell, cell target_cell) {
  if (is_out_of_bounds_cell(current_cell)) {
    FPRINTF(stderr, "Warning, can't link a NULL pointer to the target_cell!\n");
    return;
  }
  current_cell->right = target_cell;
}

/**
 * @brief Link the cell current_cell to the cell target_cell on the left
 *
 * @param current_cell, the cell which is going to be linked
 * @param target_cell, the cell to which we link current_cell to
 **/
void set_left_cell(cell current_cell, cell target_cell) {
  if (is_out_of_bounds_cell(current_cell)) {
    FPRINTF(stderr, "Warning, can't link a NULL pointer to the target_cell!\n");
    return;
  }
  current_cell->left = target_cell;
}

/**
 * @brief Link the cell current_cell to the cell target_cell above
 *
 * @param current_cell, the cell which is going to be linked
 * @param target_cell, the cell to which we link current_cell to
 **/
void set_top_cell(cell current_cell, cell target_cell) {
  if (is_out_of_bounds_cell(current_cell)) {
    FPRINTF(stderr, "Warning, can't link a NULL pointer to the target_cell!\n");
    return;
  }
  current_cell->top = target_cell;
}

/**
 * @brief Link the cell current_cell to the cell target_cell below
 *
 * @param current_cell, the cell which is going to be linked
 * @param target_cell, the cell to which we link current_cell to
 **/
void set_bottom_cell(cell current_cell, cell target_cell) {
  if (is_out_of_bounds_cell(current_cell)) {
    FPRINTF(stderr, "Warning, can't link a NULL pointer to the target_cell!\n");
    return;
  }
  current_cell->bottom = target_cell;
}

/**
 * @brief Creates a row of cells of a given width
 *
 * @param width, the desired width of the line
 * @return the left-most cell of the row
 **/
cell create_cell_line(uint16_t width) {
  if (width == 0) {
    FPRINTF(stderr, "Error: create_cell_line, given width is 0.\n");
    return make_out_of_bounds_cell();
  }

  cell origin = alloc_cell();
  if (is_out_of_bounds_cell(origin)) {
    FPRINTF(stderr, "Error: create_cell_line, can't allocate origin.\n");
    return origin;
  }

  cell previous_cell = origin;
  for (uint16_t i = 1; i < width; i++) {
    cell new_cell = alloc_cell();
    if (is_out_of_bounds_cell(new_cell)) {
      FPRINTF(stderr,
              "Error: create_cell_line, can't allocate enough cells.\n");
      destroy_cell_line(origin);
      return make_out_of_bounds_cell();
    }
    set_right_cell(previous_cell, new_cell);
    set_left_cell(new_cell, previous_cell);
    previous_cell = new_cell;
  }

  return origin;
}

/**
 * @brief Destroy a line of cells
 *
 * @param origin, the left-most cell of the row
 **/
void destroy_cell_line(cell origin) {
  cell current_cell = origin;
  uint16_t length = line_size_cell(origin);
  for (uint16_t i = 0; i < length; i++) {
    cell next_cell = get_right_cell(current_cell);
    free_cell(current_cell);
    current_cell = next_cell;
  }
}

/**
 * @brief Destroy a rectangle of cells
 *
 * @param origin, the bottomleft-most cell of the rectangle
 **/
void destroy_cell_rectangle(cell origin) {
  cell current_cell = origin;
  uint16_t height = size_column_cell(origin);
  for (uint16_t i = 0; i < height; i++) {
    cell next_cell = get_top_cell(current_cell);
    destroy_cell_line(current_cell);
    current_cell = next_cell;
  }
}

/**
 * @brief Shuffles the direction of a line of cells
 *
 * @param origin, the left-most cell of the rectangle
 **/
static void shuffle_direction_line_cell(cell origin, uint16_t width) {
  cell current_cell = origin;
  for (uint16_t i = 0; i < width; i++) {
    direction new_direction = rand() % NB_DIR;
    set_current_direction_cell(current_cell, new_direction);
    current_cell = get_right_cell(current_cell);
  }
}

/**
 * @brief Shuffles the direction of a rectangle of cells
 *
 * @param origin, the bottomleft-most cell of the rectangle
 **/
void shuffle_direction_cell(cell origin, uint16_t width, uint16_t height) {
  cell current_cell = origin;
  for (uint16_t i = 0; i < height; i++) {
    shuffle_direction_line_cell(current_cell, width);
    current_cell = get_top_cell(current_cell);
  }
}

bool link_lines_cell(cell bottom_origin, cell top_origin) {
  uint16_t bottom_width = line_size_cell(bottom_origin);
  uint16_t top_width = line_size_cell(top_origin);
  if (bottom_width != top_width) {
    FPRINTF(
        stderr,
        "Error: link_lines_cell, cannot link two lines of different size.\n");
    return false;
  }

  if (bottom_width == 0 && top_width == 0) {
    return false;
  }

  cell temporary_bottom_cell = bottom_origin;
  cell temporary_top_cell = top_origin;
  for (uint16_t i = 0; i < top_width; i++) {
    set_top_cell(temporary_bottom_cell, temporary_top_cell);
    set_bottom_cell(temporary_top_cell, temporary_bottom_cell);

    temporary_bottom_cell = get_right_cell(temporary_bottom_cell);
    temporary_top_cell = get_right_cell(temporary_top_cell);
  }
  return true;
}

bool link_columns_cell(cell left_origin, cell right_origin) {
  uint16_t left_height = size_column_cell(left_origin);
  uint16_t right_height = size_column_cell(right_origin);
  if (left_height != right_height) {
    FPRINTF(stderr,
            "Error: link_column_cell, cannot link two columns of different "
            "sizes %hu (left) and %hu (right).\n",
            left_height, right_height);
    return false;
  }

  if (left_height == 0 && right_height == 0) {
    return false;
  }

  cell temporary_left_cell = left_origin;
  cell temporary_right_cell = right_origin;
  for (uint16_t i = 0; i < left_height; i++) {
    set_left_cell(temporary_right_cell, temporary_left_cell);
    set_right_cell(temporary_left_cell, temporary_right_cell);

    temporary_left_cell = get_top_cell(temporary_left_cell);
    temporary_right_cell = get_top_cell(temporary_right_cell);
  }
  return true;
}

// cell create_rectangle_cell(uint16_t width, uint16_t height, bool is_wrapped)
// {}

static bool is_line_valid(cell origin) {
  uint16_t width = line_size_cell(origin);
  if (width < MIN_GAME_WIDTH) {
    FPRINTF(stderr,
            "Error: is_line_valid, line size is too small for a gane object, "
            "got %hu instead of at least %hu.\n",
            width, MIN_GAME_WIDTH);
    return false;
  }

  cell current_cell = get_right_cell(origin);
  cell previous_cell = origin;
  for (uint16_t i = 1; i < width; i++) {
    if (get_left_cell(current_cell) != previous_cell) {
      FPRINTF(stderr,
              "Error: is_line_valid, left neighbor of current cell is not the "
              "previous cell.\n");
      return false;
    }
    previous_cell = current_cell;
    current_cell = get_right_cell(current_cell);
  }

  return true;
}

static bool is_column_valid(cell origin) {
  uint16_t height = size_column_cell(origin);
  if (height < MIN_GAME_HEIGHT) {
    FPRINTF(stderr,
            "Error: is_column_valid, column is too small for a gane object, "
            "got %hu instead of at least %hu.\n",
            height, MIN_GAME_HEIGHT);
    return false;
  }

  cell current_cell = get_top_cell(origin);
  cell previous_cell = origin;
  for (uint16_t i = 1; i < height; i++) {
    if (get_bottom_cell(current_cell) != previous_cell) {
      FPRINTF(
          stderr,
          "Error: is_column_valid, bottom neighbor of current cell is not the "
          "previous cell.\n");
      return false;
    }
    previous_cell = current_cell;
    current_cell = get_top_cell(current_cell);
  }

  return true;
}

bool is_rectangle_valid(cell origin, bool is_wrapped) {
  uint16_t width = line_size_cell(origin);
  if (width < MIN_GAME_WIDTH) {
    FPRINTF(stderr,
            "Error: is_rectangle_valid, rectangle width is too small for a "
            "gane object, "
            "got %hu instead of at least %hu.\n",
            width, MIN_GAME_WIDTH);
    return false;
  }

  uint16_t height = size_column_cell(origin);
  if (height < MIN_GAME_HEIGHT) {
    FPRINTF(stderr,
            "Error: is_rectangle_valid, rectangle height is too small for a "
            "gane object, "
            "got %hu instead of at least %hu.\n",
            height, MIN_GAME_HEIGHT);
    return false;
  }

  cell current_column = origin;
  for (uint16_t i = 0; i < width; i++) {
    if (!is_column_valid(current_column)) {
      FPRINTF(stderr, "Error: is_rectangle_valid, column #%hu is invalid.\n",
              i);
      return false;
    }
    current_column = get_right_cell(current_column);
  }

  cell current_line = origin;
  for (uint16_t i = 0; i < height; i++) {
    if (!is_line_valid(current_line)) {
      FPRINTF(stderr, "Error: is_rectangle_valid, line #%hu is invalid.\n", i);
      return false;
    }
    current_line = get_top_cell(current_line);
  }

  if (is_wrapped) {
    cell offset_origin = translate_cell(origin, 1, 1);
    if (!is_rectangle_valid(offset_origin, false)) {
      FPRINTF(stderr, "Error: is_rectangle_valid, wrapping is invalid.\n");
      return false;
    }
  }

  return true;
}

void restore_initial_direction_cell(cell current_cell) {
  if (is_out_of_bounds_cell(current_cell)) {
    FPRINTF(stderr,
            "Error: restore_initial_direction_cell, current cell is NULL.\n");
    return;
  }
  direction initial_direction = get_default_direction_cell(current_cell);
  set_current_direction_cell(current_cell, initial_direction);
}
