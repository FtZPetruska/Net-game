#include "game.h"

#include <assert.h>
#include <stdio.h>

#include "bool_array.h"
#include "cell.h"

#define NB_DIR 4
#define DEFAULT_SIZE 5

//--------------------------------------------------------------------------------------
//                                Structures

// game board structure:
struct game_s {
  bool is_wrapped;  // boolean to know if the game is in tore mode.
  uint16_t width;
  uint16_t height;
  cell origin;  // first cell at (0,0) (down-left corner), all other cells of
                // the board will be linked from this point.
};

//--------------------------------------------------------------------------------------
//                                Static functions
//                         These functions are primitives to use cell and game
//                         correcly in net_text function

static cell get_game_origin(cgame board);
static void set_game_origin(game board, cell c);
static void set_game_height(game board, uint16_t height);
static void set_game_width(game board, uint16_t width);
static uint16_t get_game_height(cgame board);
static uint16_t get_game_width(cgame board);
static void set_game_wrap(game board, bool is_wrapped);
static bool get_game_wrap(cgame board);

static void get_coordinates_from_direction(direction dir, int *x, int *y);
static bool is_branch_over(cgame g, cell c, direction dir, bool **checked,
                           int x, int y);

game new_game_empty() {
  game board = new_game_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false);
  if (!board) {
    FPRINTF(stderr, "Error: new_game_empty, couldn't create a game object.\n");
    return NULL;
  }
  return board;
}

game new_game_empty_ext(uint16_t width, uint16_t height, bool wrapping) {
  game board = (game)malloc(sizeof(struct game_s));
  if (!board) {
    FPRINTF(stderr, "Error: new_game_empty_ext, can't allocate game board.\n");
    return NULL;
  }

  set_game_width(board, width);
  set_game_height(board, height);
  set_game_wrap(board, wrapping);

  cell origin = create_cell_line(width);
  if (is_out_of_bounds_cell(origin)) {
    FPRINTF(stderr, "Error: new_game_empty_ext, can't allocate cells.\n");
    free(board);
    return NULL;
  }

  cell previous_line_origin = origin;
  cell next_line_origin = make_out_of_bounds_cell();

  for (uint16_t column = 1; column < height; column++) {
    next_line_origin = create_cell_line(width);
    if (is_out_of_bounds_cell(next_line_origin)) {
      FPRINTF(stderr, "Error: new_game_empty_ext, can't allocate cells.\n");
      destroy_cell_rectangle(origin);
      free(board);
      return NULL;
    }

    if (!link_lines_cell(previous_line_origin, next_line_origin)) {
      FPRINTF(stderr, "Error: new_game_empty_ext, can't link two lines.\n");
      destroy_cell_line(next_line_origin);
      destroy_cell_rectangle(origin);
      free(board);
      return NULL;
    }
    previous_line_origin = next_line_origin;
  }

  if (wrapping) {
    if (!link_lines_cell(next_line_origin, origin)) {
      FPRINTF(stderr, "Error: new_game_empty_ext, can't link two lines.\n");
      destroy_cell_rectangle(origin);
      free(board);
      return NULL;
    }
    cell right_cell = translate_cell(origin, width - 1, 0);
    if (!link_columns_cell(right_cell, origin)) {
      FPRINTF(stderr, 

          "Error: new_game_empty_ext, can't link first and last columns.\n");
      destroy_cell_rectangle(origin);
      free(board);
      return NULL;
    }
  }

  if (!is_rectangle_valid(origin, wrapping)) {
    FPRINTF(stderr, 
        "Error, new_game_empty_ext, can't validate the cell linking.\n");
    destroy_cell_rectangle(origin);
    free(board);
    return NULL;
  }

  set_game_origin(board, origin);
  return board;
}

game new_game(piece *pieces, direction *initial_directions) {
  game board = new_game_ext(DEFAULT_SIZE, DEFAULT_SIZE, pieces,
                            initial_directions, false);
  if (!board) {
    FPRINTF(stderr, "Error: new_game, couldn't create a game object.\n");
    return NULL;
  }
  return board;
}

game new_game_ext(uint16_t width, uint16_t height, piece *pieces,
                  direction *initial_directions, bool wrapping) {
  game board = new_game_empty_ext(width, height, wrapping);
  if (!board) {
    FPRINTF(stderr, "Error: new_game_ext, couldn't create a game object.\n");
    return NULL;
  }

  cell origin = get_game_origin(board);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      cell current_cell = translate_cell(origin, x, y);

      piece new_piece = pieces[x + y * width];
      set_piece_cell(current_cell, new_piece);

      direction new_direction = initial_directions[x + y * width];
      set_current_direction_cell(current_cell, new_direction);
      set_default_direction_cell(current_cell, new_direction);
    }
  }

  return board;
}

void set_piece(game board, uint16_t x, uint16_t y, piece new_piece,
               direction new_direction) {
  if (!board) {
    FPRINTF(stderr, "Error: set_piece, game pointer is NULL.\n");
    return;
  }
  cell origin = get_game_origin(board);
  cell current_cell = translate_cell(origin, x, y);
  set_piece_cell(current_cell, new_piece);
  set_current_direction_cell(current_cell, new_direction);
}

bool is_wrapping(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: is_wrapping, game pointer is NULL.\n");
    return false;
  }
  return get_game_wrap(board);
}

void shuffle_direction(game board) {
  if (!board) {
    FPRINTF(stderr, "Error: shuffle_direction, game pointer is NULL.\n");
    return;
  }
  cell origin = get_game_origin(board);
  shuffle_direction_cell(origin, game_width(board), game_height(board));
}

uint16_t game_height(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: game_height, game pointer is NULL.\n");
    return 0;
  }
  return get_game_height(board);
}

uint16_t game_width(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: game_width, game pointer is NULL.\n");
    return 0;
  }
  return get_game_width(board);
}

void rotate_piece_one(game board, uint16_t x, uint16_t y) {
  if (!board) {
    FPRINTF(stderr, "Error: rotate_piece_one, game pointer is NULL.\n");
    return;
  }
  if (x >= get_game_width(board) || y >= get_game_height(board)) {
    FPRINTF(stderr, 
        "Error: rotate_piece_one, coordinates (%hu,%hu) are out of bounds, "
        "x and y must be, respectively, in the following intervals: "
        "[0; %hu[ and [0; %hu[.\n",
        x, y, get_game_width(board), get_game_height(board));
    return;
  }
  rotate_piece(board, x, y, 1);
}

void rotate_piece(game board, uint16_t x, uint16_t y,
                  int32_t nb_cw_quarter_turn) {
  if (!board) {
    FPRINTF(stderr, "Error: rotate_piece, game pointer is NULL.\n");
    return;
  }
  if (x >= get_game_width(board) || y >= get_game_height(board)) {
    FPRINTF(stderr, 
        "Error: rotate_piece, coordinates (%hu,%hu) are out of bounds, "
        "x and y must be, respectively, in the following intervals: "
        "[0; %hu[ and [0; %hu[.\n",
        x, y, get_game_width(board), get_game_height(board));
    return;
  }

  if (nb_cw_quarter_turn < 0) {
    nb_cw_quarter_turn = NB_DIR - nb_cw_quarter_turn;
  }
  nb_cw_quarter_turn = nb_cw_quarter_turn % NB_DIR;

  cell origin = get_game_origin(board);
  cell current_cell = translate_cell(origin, x, y);
  direction old_direction = get_current_direction_cell(current_cell);
  direction new_direction = old_direction + (uint16_t)nb_cw_quarter_turn;
  set_current_direction_cell(current_cell, new_direction % NB_DIR);
}

void set_piece_current_dir(game board, uint16_t x, uint16_t y,
                           direction new_direction) {
  if (!board) {
    FPRINTF(stderr, "Error: set_piece_current_dir, game pointer is NULL.\n");
    return;
  }
  if (x >= get_game_width(board) || y >= get_game_height(board)) {
    FPRINTF(stderr, 
        "Error: set_piece_current_dir, coordinates (%hu,%hu) are out of "
        "bounds, "
        "x and y must be, respectively, in the following intervals: "
        "[0; %hu[ and [0; %hu[.\n",
        x, y, get_game_width(board), get_game_height(board));
    return;
  }
  cell origin = get_game_origin(board);
  cell current_cell = translate_cell(origin, x, y);
  set_current_direction_cell(current_cell, new_direction);
}

bool is_edge_coordinates(cgame board, uint16_t x, uint16_t y, direction dir) {
  if (!board) {
    FPRINTF(stderr, "Error: is_edge_coordinates, game pointer is NULL.\n");
    return false;
  }
  if (x >= get_game_width(board) || y >= get_game_height(board)) {
    FPRINTF(stderr, 

        "Error: is_edge_coordinates, coordinates (%hu,%hu) are out of bounds, "
        "x and y must be, respectively, in the following intervals: "
        "[0; %hu[ and [0; %hu[.\n",
        x, y, get_game_width(board), get_game_height(board));
    return false;
  }
  cell origin = get_game_origin(board);
  cell current_cell = translate_cell(origin, x, y);
  piece cell_piece = get_piece_cell(current_cell);
  direction cell_direction = get_current_direction_cell(current_cell);
  return is_edge(cell_piece, cell_direction, dir);
}

bool is_edge(piece test_piece, direction orientation, direction dir) {
  if (test_piece < EMPTY || CROSS < test_piece) {
    FPRINTF(stderr, 
        "Error: is_edge, given piece has invalid value %d, expected value "
        "in interval [%d;%d].\n",
        test_piece, EMPTY, CROSS);
    return false;
  }
  if (orientation < N || W < orientation) {
    FPRINTF(stderr, 
        "Error: is_edge, given piece orientation has invalid value %d, "
        "expected value in interval [%d;%d].\n",
        orientation, N, W);
    return false;
  }
  if (dir < N || W < dir) {
    FPRINTF(stderr, 
        "Error: is_edge, given test direction has invalid value %d, "
        "expected value in interval [%d;%d].\n",
        dir, N, W);
    return false;
  }
  switch (test_piece) {
    case EMPTY:
      return false;
    case LEAF:
      return (orientation == dir);
    case SEGMENT:
      return (orientation % 2 == dir % 2);
    case CORNER:
      return (orientation == dir || (orientation + 1) % NB_DIR == dir);
    case TEE:
      return ((orientation - 1) % NB_DIR == dir || orientation == dir ||
              (orientation + 1) % NB_DIR == dir);
    case CROSS:
      return true;
    default:
      return false;
  }
}

direction opposite_dir(direction dir) {
  if (dir < N || W < dir) {
    FPRINTF(stderr, 
        "Error: opposite_dir, given test direction has invalid value %d, "
        "expected value in interval [%d;%d].\n",
        dir, N, W);
    return (direction)-1;
  }
  const direction opposite[] = {S, W, N, E};
  return opposite[dir];
}

game copy_game(cgame source_board) {
  if (!source_board) {
    FPRINTF(stderr, "Error: copy_game, source game pointer is NULL.\n");
    return NULL;
  }

  uint16_t source_width = get_game_width(source_board);
  uint16_t source_height = get_game_height(source_board);

  piece source_pieces[source_width * source_height];
  direction source_initial_direction[source_width * source_height];
  bool source_wrapping = get_game_wrap(source_board);

  cell source_origin = get_game_origin(source_board);
  for (uint16_t x = 0; x < source_width; x++) {
    for (uint16_t y = 0; y < source_height; y++) {
      cell current_source_cell = translate_cell(source_origin, x, y);
      source_pieces[x + y * source_width] = get_piece_cell(current_source_cell);
      source_initial_direction[x + y * source_width] =
          get_default_direction_cell(current_source_cell);
    }
  }

  game board_copy = new_game_ext(source_width, source_height, source_pieces,
                                 source_initial_direction, source_wrapping);
  if (!board_copy) {
    FPRINTF(stderr, "Error: copy_game, game copy pointer is NULL.\n");
    return NULL;
  }

  cell copy_origin = get_game_origin(board_copy);
  for (uint16_t x = 0; x < source_width; x++) {
    for (uint16_t y = 0; y < source_height; y++) {
      cell current_source_cell = translate_cell(source_origin, x, y);
      direction current_source_direction =
          get_current_direction_cell(current_source_cell);

      cell current_copy_cell = translate_cell(copy_origin, x, y);
      set_current_direction_cell(current_copy_cell, current_source_direction);
    }
  }

  return board_copy;
}

void delete_game(game board) {
  if (!board) {
    FPRINTF(stderr, "Error: delete_game, game pointer is NULL.\n");
    return;
  }
  cell origin = get_game_origin(board);
  destroy_cell_rectangle(origin);
  free(board);
  board = NULL;
}

piece get_piece(cgame board, uint16_t x, uint16_t y) {
  if (!board) {
    FPRINTF(stderr, "Error: get_piece, game pointer is NULL.\n");
    return (piece)-2;
  }
  if (x >= get_game_width(board) || y >= get_game_height(board)) {
    FPRINTF(stderr, 
        "Error: get_piece, coordinates (%hu,%hu) are out of bounds, "
        "x and y must be, respectively, in the following intervals: "
        "[0; %hu[ and [0; %hu[.\n",
        x, y, get_game_width(board), get_game_height(board));
    return (piece)-2;
  }

  cell origin = get_game_origin(board);
  cell current_cell = translate_cell(origin, x, y);
  return get_piece_cell(current_cell);
}

direction get_current_dir(cgame board, uint16_t x, uint16_t y) {
  if (!board) {
    FPRINTF(stderr, "Error: get_current_dir, game pointer is NULL.\n");
    return (direction)-1;
  }
  if (x >= get_game_width(board) || y >= get_game_height(board)) {
    FPRINTF(stderr, 
        "Error: get_current_dir, coordinates (%hu,%hu) are out of bounds, "
        "x and y must be, respectively, in the following intervals: "
        "[0; %hu[ and [0; %hu[.\n",
        x, y, get_game_width(board), get_game_height(board));
    return (direction)-1;
  }

  cell origin = get_game_origin(board);
  cell current_cell = translate_cell(origin, x, y);
  return get_current_direction_cell(current_cell);
}

bool is_game_over(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: is_game_over, game pointer is NULL.\n");
    return false;
  }

  uint16_t width = get_game_width(board);
  uint16_t height = get_game_height(board);
  bool **checked_cells = alloc_double_bool_array(width, height);
  checked_cells[0][0] = true;  // The origin is always checked

  int delta_x, delta_y;
  cell origin = get_game_origin(board);
  piece origin_piece = get_piece_cell(origin);
  direction origin_current_direction = get_current_direction_cell(origin);

  for (direction dir = N; dir < NB_DIR; dir++) {  // for each direction
    if (is_edge(origin_piece, origin_current_direction,
                dir)) {  // if the cell is connected to this direction we check
                         // if the branch is well formed
      get_coordinates_from_direction(dir, &delta_x, &delta_y);
      if (!is_branch_over(board, translate_cell(origin, delta_x, delta_y),
                          opposite_dir(dir), checked_cells,
                          (delta_x + width) % width,
                          (delta_y + height) % height)) {
        // one of the branch starting from the origin isn't well formed
        free_double_bool_array(checked_cells, width);
        return false;
      }
    }
  }
  bool result = check_double_bool_array(checked_cells, width, height);
  free_double_bool_array(checked_cells, width);
  return result;
}

void restart_game(game board) {
  if (!board) {
    FPRINTF(stderr, "Error: restart_game, game pointer is NULL.\n");
    return;
  }

  uint16_t width = get_game_width(board);
  uint16_t height = get_game_height(board);

  cell origin = get_game_origin(board);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      cell current_cell = translate_cell(origin, x, y);
      restore_initial_direction_cell(current_cell);
    }
  }
}

/**
 * @brief Get the origin field of a game
 *
 * @param board, const pointer to the game object
 * @return pointer to the origin cell (0,0) or NULL in case of error
 **/
static cell get_game_origin(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: get_game_origin, game pointer is NULL.\n");
    return make_out_of_bounds_cell();
  }
  return board->origin;
}

/**
 * @brief Set the origin field of a game
 *
 * @param board, the board we want to set the origin on
 * @param new_origin, the origin of a valid rectangle of cells
 **/
static void set_game_origin(game board, cell new_origin) {
  if (!board) {
    FPRINTF(stderr, "Error: set_game_origin, game pointer is NULL.\n");
    return;
  }
  bool board_wrap = get_game_wrap(board);
  if (!is_rectangle_valid(new_origin, board_wrap)) {
    FPRINTF(stderr, 
        "Error: set_game_origin, new origin is not a valid rectangle.\n");
    return;
  }
  board->origin = new_origin;
}

/**
 * @brief Set the height field of a game
 *
 * @param board, pointer to the game object
 * @param new_height, the desired height
 **/
static void set_game_height(game board, uint16_t new_height) {
  if (!board) {
    FPRINTF(stderr, "Error: set_game_height, game pointer is NULL.\n");
    return;
  }
  if (new_height < MIN_GAME_HEIGHT || MAX_GAME_HEIGHT < new_height) {
    FPRINTF(stderr, 
        "Error: set_game_height, new height %hu is out of the allowed "
        "interval: [%hu;%hu].\n",
        new_height, MIN_GAME_HEIGHT, MAX_GAME_HEIGHT);
    return;
  }
  board->height = new_height;
}

/**
 * @brief Set the width field of a game
 *
 * @param board, pointer to the game object
 * @param new_width, the desired width
 **/
static void set_game_width(game board, uint16_t new_width) {
  if (!board) {
    FPRINTF(stderr, "Error: set_game_width, game pointer is NULL.\n");
    return;
  }
  if (new_width < MIN_GAME_WIDTH || MAX_GAME_WIDTH < new_width) {
    FPRINTF(stderr, 
        "Error: set_game_width, new height %hu is out of the allowed "
        "interval: [%hu;%hu].\n",
        new_width, MIN_GAME_WIDTH, MAX_GAME_WIDTH);
    return;
  }
  board->width = new_width;
}

/**
 * @brief Get the height field of a game
 *
 * @param board, const pointer to the game object
 * @return board's height or 0 in case of error
 **/
static uint16_t get_game_height(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: get_game_height, game pointer is NULL.\n");
    return 0;
  }
  return board->height;
}

/**
 * @brief Get the width field of a board
 *
 * @param board, const pointer to the game object
 * @return board's width or 0 in case of error
 **/
static uint16_t get_game_width(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: get_game_width, game pointer is NULL.\n");
    return 0;
  }
  return board->width;
}

/**
 * @brief Set the is_wrapped field of a game
 *
 * @param board, pointer to the game object
 * @param new_wrap, the new wrapping value
 **/
static void set_game_wrap(game board, bool new_wrap) {
  if (!board) {
    FPRINTF(stderr, "Error: set_game_wrap, game pointer is NULL.\n");
    return;
  }
  board->is_wrapped = new_wrap;
}

/**
 * @brief Get the is_wrapped field of a game
 *
 * @param board, const pointer to the game object
 * @return wrap boolean or -1 in case of error
 **/
static bool get_game_wrap(cgame board) {
  if (!board) {
    FPRINTF(stderr, "Error: set_game_wrap, game pointer is NULL.\n");
    return -1;
  }
  return board->is_wrapped;
}

//--------------- utilities functions for is_game_over() --------------

/**
 * @brief Transform a direction into coordinates
 *
 * @param[in] dir, the direction we want to convert
 * @param[out] x, a pointer to the x value of the coordinates
 * @param[out] y, a pointer to the y value of the coordinates
 */
static void get_coordinates_from_direction(direction dir, int *delta_x,
                                           int *delta_y) {
  if (!delta_x || !delta_y) {
    FPRINTF(stderr, 

        "Error: get_coordinates_from_direction, out parameters are NULL.\n");
    return;
  }
  *delta_x = 0;
  *delta_y = 0;
  switch (dir) {
    case N:
      *delta_y = 1;
      break;
    case S:
      *delta_y = -1;
      break;
    case E:
      *delta_x = 1;
      break;
    case W:
      *delta_x = -1;
      break;
    default:
      FPRINTF(stderr, "Error: get_coordinates_from_direction, unknown direction");
      break;
  }
}

/**
 * @brief Recursive function to check if a branch (a series of connected cells)
 *is well connected, ends by a LEAF and contain no loop (this function is used
 *in is_game_over)
 *
 * @param board, const pointer to the game object in which the branch is located
 * @param branch_cell, the cell from where the branch start
 * @param origin_direction, the direction from where the branch come from
 * @param checked_cells, a double array of boolean to verify if the cell has
 *already been checked or not
 * @param x, the x coordinate of the branch cell
 * @param y, the y coordinate of the branch cell
 * @return true if the branch is well formed, false otherwise
 **/
static bool is_branch_over(cgame board, cell branch_cell,
                           direction origin_direction, bool **checked_cells,
                           int x, int y) {
  piece branch_piece = get_piece_cell(branch_cell);
  direction branch_current_direction = get_current_direction_cell(branch_cell);

  if (is_out_of_bounds_cell(branch_cell) ||
      !is_edge(branch_piece, branch_current_direction, origin_direction)) {
    // the branch is disconnected or doesn't end by a leaf
    return false;
  }

  if (checked_cells[x][y]) {  // If true, there is a loop
    return false;
  }
  checked_cells[x][y] = true;

  uint16_t width = get_game_width(board);
  uint16_t height = get_game_height(board);
  int delta_x, delta_y;

  for (direction dir = N; dir < NB_DIR; dir++) {
    if (dir != origin_direction &&
        is_edge(branch_piece, branch_current_direction, dir)) {
      get_coordinates_from_direction(dir, &delta_x, &delta_y);
      cell next_branch_cell = translate_cell(branch_cell, delta_x, delta_y);

      if (!is_branch_over(board, next_branch_cell, opposite_dir(dir),
                          checked_cells, (x + delta_x + width) % width,
                          (y + delta_y + height) % height)) {
        return false;
      }
    }
  }
  return true;
}
