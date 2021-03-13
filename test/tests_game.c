#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

/**
 * @brief Checks if all the pieces of a game board corresponds to the given
 * array
 *
 * @param board, the board to check
 * @param expected_pieces, an array of the pieces to check against
 *
 * @return true if all the pieces are the same
 */
static bool compare_game_and_pieces_array(game board,
                                          const piece* expected_pieces) {
  if (!board) {
    FPRINTF(stderr,
            "Error: compare_game_and_pieces_array, game pointer is NULL.\n");
    return false;
  }
  if (!expected_pieces) {
    FPRINTF(stderr,
            "Error: compare_game_and_pieces_array, pieces array is NULL.\n");
    return false;
  }

  uint16_t width = game_width(board);
  uint16_t height = game_height(board);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      piece current_piece = get_piece(board, x, y);
      if (current_piece != expected_pieces[x + width * y]) {
        FPRINTF(stderr,
                "Error: compare_game_and_pieces_array, piece at (%hu,%hu) is "
                "%d while %d was expected.\n",
                x, y, current_piece, expected_pieces[x + width * y]);
        return false;
      }
    }
  }

  return true;
}

/**
 * @brief Checks if all the pieces' direction of a game board corresponds to the
 * given array
 *
 * @param board, the board to check
 * @param expected_directions, an array of the directions to check against
 *
 * @return true if all the directions are the same
 */
static bool compare_game_and_directions_array(
    game board, const direction* expected_directions) {
  if (!board) {
    FPRINTF(
        stderr,
        "Error: compare_game_and_directions_array, game pointer is NULL.\n");
    return false;
  }
  if (!expected_directions) {
    FPRINTF(stderr,
            "Error: compare_game_and_directions_array, directions array is "
            "NULL.\n");
    return false;
  }

  uint16_t width = game_width(board);
  uint16_t height = game_height(board);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      direction current_direction = get_current_direction(board, x, y);
      if (current_direction != expected_directions[x + width * y]) {
        FPRINTF(stderr,
                "Error: compare_game_and_directions_array, piece at (%hu,%hu) "
                "has direction "
                "%d while %d was expected.\n",
                x, y, current_direction, expected_directions[x + width * y]);
        return false;
      }
    }
  }

  return true;
}

/**
 * @brief Rotate once every piece on a game board, avoids relying on
 * shuffle_direction for tests.
 *
 * @param board, the board to rotate
 */
static void rotate_all_pieces_once(game board) {
  for (uint16_t x = 0; x < game_width(board); x++) {
    for (uint16_t y = 0; y < game_height(board); y++) {
      rotate_piece_one(board, x, y);
    }
  }
}

/**
 * @brief Creates a source board board out of a fixed set of pieces.
 * Rotates all the pieces once.
 * Copies the source board.
 * Compares board sizes.
 * Compares the board current pieces and directions.
 * Restart the copy.
 * Compares the copy to the initial pieces and direction arrays.
 */
static int test_copy_game_valid() {
  const piece source_pieces[] = {LEAF,    LEAF,    LEAF,    LEAF,    LEAF,
                                 TEE,     TEE,     TEE,     TEE,     TEE,
                                 SEGMENT, SEGMENT, SEGMENT, SEGMENT, SEGMENT,
                                 CORNER,  CORNER,  CORNER,  CORNER,  CORNER,
                                 EMPTY,   EMPTY,   EMPTY,   EMPTY,   EMPTY};
  const direction source_directions[] = {N, S, W, E, N, N, S, W, E, N, N, S, W,
                                         E, N, N, S, W, E, N, N, S, W, E, N};

  game source_board = new_game(source_pieces, source_directions);

  // By rotating all the pieces we can test the current and initial directions
  // of the copy
  rotate_all_pieces_once(source_board);

  game copied_board = copy_game(source_board);

  if (!copied_board) {
    FPRINTF(stderr,
            "Error: test_copy_game_valid, copy failed and returned a NULL "
            "pointer.\n");
    delete_game(source_board);
    return EXIT_FAILURE;
  }

  uint16_t source_width = game_width(source_board);
  uint16_t source_height = game_height(source_board);
  uint16_t copy_width = game_width(copied_board);
  uint16_t copy_height = game_height(copied_board);

  if ((source_width != copy_width) || (source_height != copy_height)) {
    FPRINTF(stderr,
            "Error: test_copy_game_valid, source board had sizes w=%hu, h=%hu "
            "while its copy has sizes w=%hu, h=%hu.\n",
            source_width, source_height, copy_width, copy_height);
    delete_game(source_board);
    delete_game(copied_board);
    return EXIT_FAILURE;
  }

  for (uint16_t x = 0; x < source_width; x++) {
    for (uint16_t y = 0; y < source_height; y++) {
      piece source_piece = get_piece(source_board, x, y);
      piece copied_piece = get_piece(copied_board, x, y);
      if (source_piece != copied_piece) {
        FPRINTF(stderr,
                "Error: test_copy_game_valid, at (%hu,%hu) copied piece is %d "
                "while %d is in source.\n",
                x, y, copied_piece, source_piece);
        delete_game(source_board);
        delete_game(copied_board);
        return EXIT_FAILURE;
      }

      direction current_source_direction =
          get_current_direction(source_board, x, y);
      direction current_copied_direction =
          get_current_direction(copied_board, x, y);
      if (current_source_direction != current_copied_direction) {
        FPRINTF(stderr,
                "Error: test_copy_game_valid, at (%hu,%hu) copied piece has "
                "direction %d while %d is the direction in source.\n",
                x, y, current_copied_direction, current_source_direction);
        delete_game(source_board);
        delete_game(copied_board);
        return EXIT_FAILURE;
      }
    }
  }

  delete_game(source_board);

  // By restarting the game, we can check if the initial directions were set
  // properly on copy
  restart_game(copied_board);

  if (!compare_game_and_pieces_array(copied_board, source_pieces)) {
    FPRINTF(stderr,
            "Error: test_copy_game_valid, copy set the wrong pieces.\n");
    delete_game(copied_board);
    return EXIT_FAILURE;
  }

  if (!compare_game_and_directions_array(copied_board, source_directions)) {
    FPRINTF(stderr,
            "Error: test_copy_game_valid, copy set the wrong initial "
            "directions.\n");
    delete_game(copied_board);
    return EXIT_FAILURE;
  }

  delete_game(copied_board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to copy a NULL pointer.
 *
 */
static int test_copy_game_null_game() {
  game fake_board = copy_game(NULL);
  if (fake_board != NULL) {
    FPRINTF(stderr,
            "Error: test_copy_game_null_game, copy game returned a valid copy "
            "when given a NULL pointer.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Creates an empty game, copy it and frees both.
 *
 */
static int test_delete_game_valid_copy() {
  game source_board = new_game_empty();
  game copied_board = copy_game(source_board);
  delete_game(source_board);
  delete_game(copied_board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates an empty game and frees it.
 *
 */
static int test_delete_game_valid_empty() {
  game board = new_game_empty();
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates an empty game with wrap and frees it.
 *
 */
static int test_delete_game_valid_empty_ext() {
  game board = new_game_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true);
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a regular game and frees it.
 *
 */
static int test_delete_game_valid_normal() {
  const piece initial_pieces[] = {
      LEAF,    TEE,  LEAF,   LEAF, LEAF, LEAF,    TEE, TEE, CORNER,
      SEGMENT, LEAF, LEAF,   TEE,  LEAF, SEGMENT, TEE, TEE, TEE,
      TEE,     TEE,  CORNER, LEAF, LEAF, CORNER,  LEAF};
  const direction initial_directions[] = {E, W, S, E, S, S, S, N, W, S, E, N, W,
                                          W, E, S, W, N, E, E, W, N, W, N, S};
  game board = new_game(initial_pieces, initial_directions);
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a regular wrapped game and frees it.
 *
 */
static int test_delete_game_valid_normal_ext() {
  uint16_t size = 3;
  const piece initial_pieces[] = {EMPTY, LEAF, SEGMENT, CORNER, TEE,
                                  CROSS, TEE,  CORNER,  SEGMENT};
  const direction initial_directions[] = {N, E, S, W, N, W, S, E, N};
  game board =
      new_game_ext(size, size, initial_pieces, initial_directions, true);
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to delete a NULL pointer.
 *
 */
static int test_delete_game_null_game() {
  delete_game(NULL);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a default empty game and checks if the returned height
 * corresponds to DEFAULT_SIZE declared in game.h.
 */
static int test_game_height_valid() {
  game board = new_game_empty();
  uint16_t height = game_height(board);
  delete_game(board);

  if (height != DEFAULT_SIZE) {
    FPRINTF(stderr,
            "Error: test_game_height_null_game, game_height returned %hu "
            "instead of 0.\n",
            height);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get the height of a NULL pointer and verifies if it returns
 * 0 as defined in game.h.
 */
static int test_game_height_null_game() {
  uint16_t fake_height = game_height(NULL);
  if (fake_height != 0) {
    FPRINTF(stderr,
            "Error: test_game_height_null_game, game_height returned %hu "
            "instead of 0.\n",
            fake_height);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a default empty game and checks if the returned width
 * corresponds to DEFAULT_SIZE declared in game.h.
 */
static int test_game_width_valid() {
  game board = new_game_empty();
  uint16_t width = game_width(board);
  delete_game(board);

  if (width != DEFAULT_SIZE) {
    FPRINTF(stderr,
            "Error: test_game_width_valid, game_width returned %hu "
            "instead of 0.\n",
            width);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get the width of a NULL pointer and verifies if it returns
 * 0 as defined in game.h.
 */
static int test_game_width_null_game() {
  uint16_t fake_width = game_width(NULL);
  if (fake_width != 0) {
    FPRINTF(stderr,
            "Error: test_game_width_null_game, game_width returned %hu "
            "instead of 0.\n",
            fake_width);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a standard game with all piece types and directions.
 * Tests if for each piece each direction is correctly initialized.
 */
static int test_get_current_direction_valid() {
  const piece default_pieces[] = {
      LEAF,   LEAF,    LEAF,    LEAF,    CROSS,   TEE,   TEE,    TEE,    TEE,
      CROSS,  SEGMENT, SEGMENT, SEGMENT, SEGMENT, CROSS, CORNER, CORNER, CORNER,
      CORNER, CROSS,   EMPTY,   EMPTY,   EMPTY,   EMPTY, EMPTY};
  const direction default_dirs[] = {N, S, W, E, N, N, S, W, E, S, N, S, W,
                                    E, S, N, S, W, E, E, N, S, W, E, N};

  game board = new_game(default_pieces, default_dirs);
  if (!compare_game_and_directions_array(board, default_dirs)) {
    FPRINTF(stderr,
            "Error: test_get_current_direction_valid, the board pieces' "
            "direction differs from default.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }

  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get the direction of NULL pointer at coordinates (0,0) and
 * verifies if it returns -1 as defined in game.h
 */
static int test_get_current_direction_null_game() {
  direction fake_direction = get_current_direction(NULL, 0, 0);
  if (fake_direction != (direction)-1) {
    FPRINTF(stderr,
            "Error: test_get_current_direction_null_game, "
            "get_current_direction returned %d while %d was expected.\n",
            fake_direction, (direction)-1);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get the direction outside the boundaries of the game and
 * verifies if it returns -1 as defined in game.h
 */
static int test_get_current_direction_out_of_bounds() {
  game board = new_game_empty();
  uint16_t out_of_bound_x = DEFAULT_SIZE;
  uint16_t out_of_bound_y = DEFAULT_SIZE;
  direction oob_direction =
      get_current_direction(board, out_of_bound_x, out_of_bound_y);

  if (oob_direction != (direction)-1) {
    FPRINTF(stderr,
            "Error: test_get_current_direction_out_of_bounds, "
            "get_current_direction returned %d while %d was expected.\n",
            oob_direction, (direction)-1);
    delete_game(board);
    return EXIT_FAILURE;
  }

  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a standard game with all piece types and directions.
 * Tests if each piece is correctly initialized.
 */
static int test_get_piece_valid() {
  const piece default_pieces[] = {
      LEAF,   LEAF,    LEAF,    LEAF,    CROSS,   TEE,   TEE,    TEE,    TEE,
      CROSS,  SEGMENT, SEGMENT, SEGMENT, SEGMENT, CROSS, CORNER, CORNER, CORNER,
      CORNER, CROSS,   EMPTY,   EMPTY,   EMPTY,   EMPTY, EMPTY};
  const direction default_dirs[] = {N, S, W, E, N, N, S, W, E, S, N, S, W,
                                    E, S, N, S, W, E, E, N, S, W, E, N};

  game board = new_game(default_pieces, default_dirs);
  if (!compare_game_and_pieces_array(board, default_pieces)) {
    FPRINTF(stderr,
            "Error: test_get_current_direction_valid, the board' pieces "
            "differs from default.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get the direction of NULL pointer at coordinates (0,0) and
 * verifies if it returns -1 as defined in game.h
 */
static int test_get_piece_null_game() {
  piece fake_piece = get_piece(NULL, 0, 0);
  if (fake_piece != (piece)-2) {
    FPRINTF(stderr,
            "Error: test_get_piece_null_game, "
            "get_piece returned %d while %d was expected.\n",
            fake_piece, (piece)-2);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get a piece outside the boundaries of the game and
 * verifies if it returns -2 as defined in game.h
 */
static int test_get_piece_out_of_bounds() {
  game board = new_game_empty();
  uint16_t out_of_bound_x = DEFAULT_SIZE;
  uint16_t out_of_bound_y = DEFAULT_SIZE;
  piece oob_piece = get_piece(board, out_of_bound_x, out_of_bound_y);

  if (oob_piece != (piece)-2) {
    FPRINTF(stderr,
            "Error: test_get_piece_out_of_bounds, "
            "get_piece returned %d while %d was expected.\n",
            oob_piece, (piece)-2);
    delete_game(board);
    return EXIT_FAILURE;
  }

  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all valid parameters of is_edge for an EMPTY piece
 */
static int test_is_edge_EMPTY() {
  for (direction i = N; i < NB_DIR; i++) {
    for (direction j = N; j < NB_DIR; j++) {
      if (is_edge(EMPTY, i, j)) {
        FPRINTF(stderr,
                "Error: test_is_edge_EMPTY, is_edge returned true for piece "
                "orientation %d in direction %d for an EMPTY piece.\n",
                i, j);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all valid parameters of is_edge for a LEAF piece
 */
static int test_is_edge_LEAF() {
  for (direction i = N; i < NB_DIR; i++) {
    for (direction j = N; j < NB_DIR; j++) {
      bool result = is_edge(LEAF, i, j);
      if (result != (i == j)) {
        FPRINTF(stderr,
                "Error: test_is_edge_LEAF, is_edge returned %s for piece "
                "orientation %d in direction %d for a LEAF piece.\n",
                result ? "true" : "false", i, j);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all valid parameters of is_edge for a SEGMENT piece
 */
static int test_is_edge_SEGMENT() {
  for (direction i = N; i < NB_DIR; i++) {
    for (direction j = N; j < NB_DIR; j++) {
      bool result = is_edge(SEGMENT, i, j);
      if (result != (i % 2 == j % 2)) {
        FPRINTF(stderr,
                "Error: test_is_edge_SEGMENT, is_edge returned %s for piece "
                "orientation %d in direction %d for a SEGMENT piece.\n",
                result ? "true" : "false", i, j);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all valid parameters of is_edge for a CORNER piece
 */
static int test_is_edge_CORNER() {
  for (direction i = N; i < NB_DIR; i++) {
    for (direction j = N; j < NB_DIR; j++) {
      bool result = is_edge(CORNER, i, j);
      if (result != (i == j || (i + 1) % NB_DIR == j)) {
        FPRINTF(stderr,
                "Error: test_is_edge_CORNER, is_edge returned %s for piece "
                "orientation %d in direction %d for a CORNER piece.\n",
                result ? "true" : "false", i, j);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all valid parameters of is_edge for a TEE piece
 */
static int test_is_edge_TEE() {
  for (direction i = N; i < NB_DIR; i++) {
    for (direction j = N; j < NB_DIR; j++) {
      bool result = is_edge(TEE, i, j);
      if (result !=
          ((i - 1) % NB_DIR == j || i == j || (i + 1) % NB_DIR == j)) {
        FPRINTF(stderr,
                "Error: test_is_edge_TEE, is_edge returned %s for piece "
                "orientation %d in direction %d for a TEE piece.\n",
                result ? "true" : "false", i, j);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all valid parameters of is_edge for a CROSS piece
 */
static int test_is_edge_CROSS() {
  for (direction i = N; i < NB_DIR; i++) {
    for (direction j = N; j < NB_DIR; j++) {
      if (!is_edge(CROSS, i, j)) {
        FPRINTF(stderr,
                "Error: test_is_edge_CORNER, is_edge returned false for piece "
                "orientation %d in direction %d for a CROSS piece.\n",
                i, j);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all directions for is_edge_coordinates on an EMPTY piece
 */
static int test_is_edge_coordinates_EMPTY() {
  const piece default_pieces[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                  EMPTY, EMPTY, EMPTY, EMPTY};
  const direction default_dirs[] = {N, E, S, W, N, E, S, W, N};
  uint16_t board_size = 3;
  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);
  for (uint16_t x = 0; x < board_size; x++) {
    for (uint16_t y = 0; y < board_size; y++) {
      for (direction test_direction = N; test_direction < NB_DIR;
           test_direction++) {
        if (is_edge_coordinates(board, x, y, test_direction)) {
          FPRINTF(stderr,
                  "Error: test_is_edge_coordinates_EMPTY, is_edge_coordinates "
                  "returned true for the piece (%hu,%hu) in the direction %d "
                  "for an EMPTY piece.\n",
                  x, y, test_direction);
          delete_game(board);
          return EXIT_FAILURE;
        }
      }
    }
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all directions for is_edge_coordinates on an LEAF piece
 */
static int test_is_edge_coordinates_LEAF() {
  const piece default_pieces[] = {LEAF, LEAF, LEAF, LEAF, LEAF,
                                  LEAF, LEAF, LEAF, LEAF};
  const direction default_dirs[] = {N, E, S, W, N, E, S, W, N};
  uint16_t board_size = 3;
  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);
  for (uint16_t x = 0; x < board_size; x++) {
    for (uint16_t y = 0; y < board_size; y++) {
      for (direction test_direction = N; test_direction < NB_DIR;
           test_direction++) {
        direction current_direction = get_current_direction(board, x, y);
        bool result = is_edge_coordinates(board, x, y, test_direction);
        if (result != (current_direction == test_direction)) {
          FPRINTF(stderr,
                  "Error: test_is_edge_coordinates_LEAF, is_edge_coordinates "
                  "returned %s for the piece (%hu,%hu) in the direction %d "
                  "for a LEAF piece.\n",
                  result ? "true" : "false", x, y, test_direction);
          delete_game(board);
          return EXIT_FAILURE;
        }
      }
    }
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all directions for is_edge_coordinates on a SEGMENT piece
 */
static int test_is_edge_coordinates_SEGMENT() {
  const piece default_pieces[] = {SEGMENT, SEGMENT, SEGMENT, SEGMENT, SEGMENT,
                                  SEGMENT, SEGMENT, SEGMENT, SEGMENT};
  const direction default_dirs[] = {N, E, S, W, N, E, S, W, N};
  uint16_t board_size = 3;
  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);
  for (uint16_t x = 0; x < board_size; x++) {
    for (uint16_t y = 0; y < board_size; y++) {
      for (direction test_direction = N; test_direction < NB_DIR;
           test_direction++) {
        direction current_direction = get_current_direction(board, x, y);
        bool result = is_edge_coordinates(board, x, y, test_direction);
        if (result != (current_direction % 2 == test_direction % 2)) {
          FPRINTF(
              stderr,
              "Error: test_is_edge_coordinates_SEGMENT, is_edge_coordinates "
              "returned %s for the piece (%hu,%hu) in the direction %d "
              "for a SEGMENT piece.\n",
              result ? "true" : "false", x, y, test_direction);
          delete_game(board);
          return EXIT_FAILURE;
        }
      }
    }
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all directions for is_edge_coordinates on a CORNER piece
 */
static int test_is_edge_coordinates_CORNER() {
  const piece default_pieces[] = {CORNER, CORNER, CORNER, CORNER, CORNER,
                                  CORNER, CORNER, CORNER, CORNER};
  const direction default_dirs[] = {N, E, S, W, N, E, S, W, N};
  uint16_t board_size = 3;
  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);
  for (uint16_t x = 0; x < board_size; x++) {
    for (uint16_t y = 0; y < board_size; y++) {
      for (direction test_direction = N; test_direction < NB_DIR;
           test_direction++) {
        direction current_direction = get_current_direction(board, x, y);
        bool result = is_edge_coordinates(board, x, y, test_direction);
        if (result != (current_direction == test_direction ||
                       (current_direction + 1) % NB_DIR == test_direction)) {
          FPRINTF(stderr,
                  "Error: test_is_edge_coordinates_CORNER, is_edge_coordinates "
                  "returned %s for the piece (%hu,%hu) in the direction %d "
                  "for a CORNER piece.\n",
                  result ? "true" : "false", x, y, test_direction);
          delete_game(board);
          return EXIT_FAILURE;
        }
      }
    }
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all directions for is_edge_coordinates on a TEE piece
 */
static int test_is_edge_coordinates_TEE() {
  const piece default_pieces[] = {TEE, TEE, TEE, TEE, TEE, TEE, TEE, TEE, TEE};
  const direction default_dirs[] = {N, E, S, W, N, E, S, W, N};
  uint16_t board_size = 3;
  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);
  for (uint16_t x = 0; x < board_size; x++) {
    for (uint16_t y = 0; y < board_size; y++) {
      for (direction test_direction = N; test_direction < NB_DIR;
           test_direction++) {
        direction current_direction = get_current_direction(board, x, y);
        bool result = is_edge_coordinates(board, x, y, test_direction);
        if (result != ((current_direction - 1) % NB_DIR == test_direction ||
                       current_direction == test_direction ||
                       (current_direction + 1) % NB_DIR == test_direction)) {
          FPRINTF(stderr,
                  "Error: test_is_edge_coordinates_TEE, is_edge_coordinates "
                  "returned %s for the piece (%hu,%hu) in the direction %d "
                  "for a TEE piece.\n",
                  result ? "true" : "false", x, y, test_direction);
          delete_game(board);
          return EXIT_FAILURE;
        }
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tests all directions for is_edge_coordinates on a CROSS piece
 */
static int test_is_edge_coordinates_CROSS() {
  const piece default_pieces[] = {CROSS, CROSS, CROSS, CROSS, CROSS,
                                  CROSS, CROSS, CROSS, CROSS};
  const direction default_dirs[] = {N, E, S, W, N, E, S, W, N};
  uint16_t board_size = 3;
  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);
  for (uint16_t x = 0; x < board_size; x++) {
    for (uint16_t y = 0; y < board_size; y++) {
      for (direction test_direction = N; test_direction < NB_DIR;
           test_direction++) {
        if (!is_edge_coordinates(board, x, y, test_direction)) {
          FPRINTF(stderr,
                  "Error: test_is_edge_coordinates_CROSS, is_edge_coordinates "
                  "returned false for the piece (%hu,%hu) in the direction %d "
                  "for an CROSS piece.\n",
                  x, y, test_direction);
          delete_game(board);
          return EXIT_FAILURE;
        }
      }
    }
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a completed regular board game and verifies that is_game_over
 * returns true.
 */
static int test_is_game_over_true() {
  const piece default_pieces[] = {LEAF,   CORNER, LEAF, TEE, CROSS,
                                  CORNER, LEAF,   LEAF, LEAF};
  const direction default_dirs[] = {N, N, W, E, S, W, S, S, S};
  const uint16_t board_size = 3;

  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);

  if (!is_game_over(board)) {
    FPRINTF(stderr,
            "Error: test_is_game_over_true, is_game_over returned false on a "
            "completed board.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a completed wrapped board game and verifies that is_game_over
 * returns true.
 */
static int test_is_game_over_true_wrapped() {
  const piece default_pieces[] = {LEAF,   CORNER, LEAF,   TEE, SEGMENT,
                                  CORNER, CORNER, CORNER, LEAF};
  const direction default_dirs[] = {N, E, W, E, E, W, E, W, S};
  const uint16_t board_size = 3;

  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, true);
  if (!is_game_over(board)) {
    FPRINTF(stderr,
            "Error: test_is_game_over_true_wrapped, is_game_over returned "
            "false on a "
            "completed board.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a completed regular board game, rotates all the pieces once,
 * and verifies that is_game_over returns false.
 */
static int test_is_game_over_false() {
  const piece default_pieces[] = {LEAF,   CORNER, LEAF, TEE, CROSS,
                                  CORNER, LEAF,   LEAF, LEAF};
  const direction default_dirs[] = {N, N, W, E, S, W, S, S, S};
  const uint16_t board_size = 3;

  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, false);
  rotate_all_pieces_once(board);
  if (is_game_over(board)) {
    FPRINTF(stderr,
            "Error: test_is_game_over_false, is_game_over returned true on an "
            "incompleted board.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a completed wrapped board game, rotates all the pieces once,
 * and verifies that is_game_over returns false.
 */
static int test_is_game_over_false_wrapped() {
  const piece default_pieces[] = {LEAF,   CORNER, LEAF,   TEE, SEGMENT,
                                  CORNER, CORNER, CORNER, LEAF};
  const direction default_dirs[] = {N, E, W, E, E, W, E, W, S};
  const uint16_t board_size = 3;

  game board =
      new_game_ext(board_size, board_size, default_pieces, default_dirs, true);
  rotate_all_pieces_once(board);
  if (is_game_over(board)) {
    FPRINTF(stderr,
            "Error: test_is_game_over_false_wrapped, is_game_over returned "
            "true on an "
            "incompleted board.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates an empty game and verifies that is_game_over returns false.
 */
static int test_is_game_over_empty_game() {
  game board = new_game_empty();
  if (is_game_over(board)) {
    FPRINTF(stderr,
            "Error: is_game_over_empty_game, is_game_over returned true on an "
            "empty game.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to check if a NULL pointer is completed and verifies if it
 * returns false as defined in game.h.
 */
static int test_is_game_over_null_game() {
  if (is_game_over(NULL)) {
    FPRINTF(stderr,
            "Error: is_game_over_empty_game, is_game_over returned true on a "
            "NULL pointer.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Creates an empty wrapped game and checks if the wrap value is set
 * correctly.
 */
static int test_is_wrapping_true() {
  game board = new_game_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true);
  if (!is_wrapping(board)) {
    FPRINTF(stderr,
            "Error: test_is_wrapping_true, is_wrapping returned false on a "
            "wrapped game.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Creates an empty regular game and checks if the wrap value is set
 * correctly.
 */
static int test_is_wrapping_false() {
  game board = new_game_empty();
  if (is_wrapping(board)) {
    FPRINTF(stderr,
            "Error: test_is_wrapping_false, is_wrapping returned true on a "
            "regular game.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }
  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get the wrapping of a NULL pointer and verifies if it returns
 * false as defined in game.h.
 */
static int test_is_wrapping_null_game() {
  if (is_wrapping(NULL)) {
    FPRINTF(stderr,
            "Error: test_is_wrapping_null_game, is_wrapping returned true on a "
            "NULL pointer.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_new_game() {
  piece default_pieces[] = {LEAF,    TEE,    LEAF,    LEAF, LEAF, LEAF, TEE,
                            TEE,     CORNER, SEGMENT, LEAF, LEAF, TEE,  LEAF,
                            SEGMENT, TEE,    TEE,     TEE,  TEE,  TEE,  CORNER,
                            LEAF,    LEAF,   CORNER,  LEAF};
  direction default_dirs[] = {E, W, S, E, S, S, S, N, W, S, E, N, W,
                              W, E, S, W, N, E, E, W, N, W, N, S};
  game g = new_game(default_pieces, default_dirs);
  uint16_t width = DEFAULT_SIZE;
  uint16_t height = DEFAULT_SIZE;
  if (width != game_width(g) || height != game_height(g)) {
    FPRINTF(stderr, "Error: width or height is not at default size!\n");
    delete_game(g);
    return EXIT_FAILURE;
  }
  for (uint16_t i = 0; i < width; i++) {
    for (uint16_t j = 0; j < height; j++) {
      if (get_piece(g, i, j) != default_pieces[(j * width) + i]) {
        FPRINTF(
            stderr,
            "Error: piece (%hu,%hu) is not corresponding! (should be %d, is "
            "%d)\n",
            i, j, default_pieces[(j * width) + i], get_piece(g, i, j));
        delete_game(g);
        return EXIT_FAILURE;
      }
      if (get_current_direction(g, i, j) != default_dirs[(j * width) + i]) {
        FPRINTF(stderr,
                "Error: piece's direction (%hu,%hu) is not corresponding! "
                "(should be %d, is %d)\n",
                i, j, default_dirs[(j * width) + i],
                get_current_direction(g, i, j));
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  delete_game(g);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_new_game_empty() {
  game g = new_game_empty();
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);

  if (!g) {
    FPRINTF(stderr, "Returned game is NULL\n");
    return EXIT_FAILURE;
  }

  if (width != DEFAULT_SIZE || height != DEFAULT_SIZE) {
    FPRINTF(stderr, "Invalid game size\n");
    delete_game(g);
    return EXIT_FAILURE;
  }

  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      if (get_piece(g, x, y) != EMPTY) {
        FPRINTF(
            stderr,
            "Error: test_new_game_empty, piece at (%hu,%hu) is %d, expected "
            "%d.\n",
            x, y, get_piece(g, x, y), EMPTY);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  delete_game(g);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_new_game_empty_ext() {
  uint16_t width = 42;
  uint16_t height = 115;
  for (uint8_t wrapping = 0; wrapping < 2; wrapping++) {
    game g = new_game_empty_ext(width, height, wrapping);

    if (!g) {
      FPRINTF(stderr,
              "Error with new_game_empty_ext : returned game is NULL\n");
      delete_game(g);
      return EXIT_FAILURE;
    }

    if (game_width(g) != width || game_height(g) != height) {
      FPRINTF(stderr, "Error with new_game_empty_ext : invalid game size\n");
      delete_game(g);
      return EXIT_FAILURE;
    }

    for (uint16_t x = 0; x < width; x++) {
      for (uint16_t y = 0; y < height; y++) {
        if (get_piece(g, x, y) != EMPTY) {
          FPRINTF(stderr, "Error with new_game_empty_ext : piece not empty\n");
          delete_game(g);
          return EXIT_FAILURE;
        }
      }
    }
    if (is_wrapping(g) != wrapping) {
      FPRINTF(stderr,
              "Error with new_game_empty_ext : wrapping was expected to be %s, "
              "but is %s\n",
              wrapping ? "true" : "false", is_wrapping(g) ? "true" : "false");
      delete_game(g);
      return EXIT_FAILURE;
    }
    delete_game(g);
  }

  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_new_game_ext() {
  piece default_pieces[] = {LEAF,   CORNER, CORNER, CORNER, SEGMENT,
                            CORNER, LEAF,   CORNER, SEGMENT};
  direction default_dirs[] = {N, E, S, E, E, W, E, W, N};
  uint16_t width = 3;
  uint16_t height = 3;

  for (uint8_t wrapping = 0; wrapping < 2; wrapping++) {
    game g =
        new_game_ext(width, height, default_pieces, default_dirs, wrapping);

    if (width != game_width(g) || height != game_height(g)) {
      FPRINTF(stderr, "Error with new_game_ext : invalid game size\n");
      delete_game(g);
      return EXIT_FAILURE;
    }

    for (uint16_t x = 0; x < width; x++) {
      for (uint16_t y = 0; y < height; y++) {
        if (get_piece(g, y, x) != default_pieces[(x * width) + y]) {
          FPRINTF(stderr,
                  "Error with new_game_ext : piece (%d,%d) is not "
                  "corresponding! (should be %d, is "
                  "%d)\n",
                  y, x, default_pieces[(x * width) + y], get_piece(g, y, x));
          delete_game(g);
          return EXIT_FAILURE;
        }
        if (get_current_direction(g, y, x) != default_dirs[(x * width) + y]) {
          FPRINTF(stderr,
                  "Error with new_game_ext : piece's direction (%d,%d) is not "
                  "corresponding! "
                  "(should be %d, is %d)\n",
                  y, x, default_dirs[(x * width) + y],
                  get_current_direction(g, y, x));
          delete_game(g);
          return EXIT_FAILURE;
        }
      }
    }

    if (is_wrapping(g) != wrapping) {
      FPRINTF(stderr,
              "Error with new_game_ext : wrapping was expected to be %s, "
              "but is %s\n",
              wrapping ? "true" : "false", is_wrapping(g) ? "true" : "false");
      delete_game(g);
      return EXIT_FAILURE;
    }

    delete_game(g);
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Checks if the opposite of N is S
 */
static int test_opposite_direction_N() {
  if (opposite_direction(N) != S) {
    FPRINTF(
        stderr,
        "Error: test_opposite_direction_N, opposite_direction returned %d as "
        "the opposite of %d while %d was expected.\n",
        opposite_direction(N), N, S);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Checks if the opposite of E is W
 */
static int test_opposite_direction_E() {
  if (opposite_direction(E) != W) {
    FPRINTF(
        stderr,
        "Error: test_opposite_direction_E, opposite_direction returned %d as "
        "the opposite of %d while %d was expected.\n",
        opposite_direction(E), E, W);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Checks if the opposite of S is N
 */
static int test_opposite_direction_S() {
  if (opposite_direction(S) != N) {
    FPRINTF(
        stderr,
        "Error: test_opposite_direction_S, opposite_direction returned %d as "
        "the opposite of %d while %d was expected.\n",
        opposite_direction(S), S, N);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Checks if the opposite of W is E
 */
static int test_opposite_direction_W() {
  if (opposite_direction(W) != E) {
    FPRINTF(
        stderr,
        "Error: test_opposite_direction_W, opposite_direction returned %d as "
        "the opposite of %d while %d was expected.\n",
        opposite_direction(W), W, E);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief Tries to get the opposite direction of an invalid value and verifies
 * if it returns -1 as defined in game.h
 */
static int test_opposite_direction_invalid() {
  if (opposite_direction(INT16_MAX) != (direction)-1) {
    FPRINTF(stderr,
            "Error: test_opposite_direction_invalid, opposite_direction "
            "returned %d as "
            "the opposite of %d while %d was expected.\n",
            opposite_direction(INT16_MAX), INT16_MAX, (direction)-1);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_restart_game() {
  piece default_pieces[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                            EMPTY, EMPTY, EMPTY, EMPTY};
  direction default_dirs[] = {S, S, S, S, S, S, S, S, S, S, S, S, S,
                              S, S, S, S, S, S, S, S, S, S, S, S};
  game g = new_game(default_pieces, default_dirs);
  shuffle_direction(g);
  restart_game(g);
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      if (get_piece(g, x, y) != default_pieces[x + y * DEFAULT_SIZE] ||
          get_current_direction(g, x, y) !=
              default_dirs[x + y * DEFAULT_SIZE]) {
        FPRINTF(stderr,
                "Error with restart_game : after a restart the piece "
                "was %d and should be %d and the dir was %d and should be %d "
                "@(x=%d,y=%d)\n",
                get_piece(g, x, y), default_pieces[x + y * DEFAULT_SIZE],
                get_current_direction(g, x, y),
                default_dirs[x + y * DEFAULT_SIZE], x, y);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  delete_game(g);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_rotate_piece() {
  game g = new_game_empty();
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  direction dir;
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      for (int32_t i = 0; i < 8; i++) {
        dir = get_current_direction(g, x, y);
        rotate_piece(g, x, y, i);
        if (get_current_direction(g, x, y) != (dir + i) % 4) {
          FPRINTF(stderr,
                  "Error: function void rotate_piece(game game, int x, int y, "
                  "int cnb_cw_quarter_turn) is not working correctly\n");
          delete_game(g);
          return EXIT_FAILURE;
        }
      }
    }
  }
  delete_game(g);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_rotate_piece_one() {
  bool isGood = true;
  piece default_pieces[] = {LEAF,    LEAF,    LEAF,    LEAF,    LEAF,
                            TEE,     TEE,     TEE,     TEE,     TEE,
                            SEGMENT, SEGMENT, SEGMENT, SEGMENT, SEGMENT,
                            CORNER,  CORNER,  CORNER,  CORNER,  CORNER,
                            EMPTY,   EMPTY,   EMPTY,   EMPTY,   EMPTY};
  direction default_dirs[] = {N, S, W, E, N, N, S, W, E, N, N, S, W,
                              E, N, N, S, W, E, N, N, S, W, E, N};

  game board = new_game(default_pieces, default_dirs);

  uint16_t width = game_width(board);
  uint16_t height = game_height(board);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      direction dir = get_current_direction(board, x, y);
      rotate_piece_one(board, x, y);
      switch (get_current_direction(board, x, y)) {
        case N:
          isGood = (dir == W);
          break;
        case S:
          isGood = (dir == E);
          break;
        case W:
          isGood = (dir == S);
          break;
        case E:
          isGood = (dir == N);
          break;
      };
    }
  }

  delete_game(board);
  if (isGood) {
    return EXIT_SUCCESS;
  } else {
    FPRINTF(stderr, "Rotation problem\n");
    return EXIT_FAILURE;
  }
}

/**
 * @brief
 */
static int test_set_piece() {
  const piece default_pieces[] = {
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
  const direction default_dirs[] = {S, S, S, S, S, S, S, S, S, S, S, S, S,
                                    S, S, S, S, S, S, S, S, S, S, S, S};
  game g = new_game(default_pieces, default_dirs);
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece(g, x, y, EMPTY, S);
    }
  }
  restart_game(g);
  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      if (get_piece(g, x, y) != default_pieces[x + y * DEFAULT_SIZE] ||
          get_current_direction(g, x, y) !=
              default_dirs[x + y * DEFAULT_SIZE]) {
        FPRINTF(stderr,
                "Error when set_piece : after a restart the piece "
                "was %d and should be %d and the dir was %d and should be %d "
                "@(x=%d,y=%d)\n",
                get_piece(g, x, y), default_pieces[x + y * DEFAULT_SIZE],
                get_current_direction(g, x, y),
                default_dirs[x + y * DEFAULT_SIZE], x, y);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  delete_game(g);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_set_piece_current_dir() {
  game g = new_game_empty();
  direction dir = N;
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece_current_direction(g, x, y, dir);
      if (get_current_direction(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_direction : Direction is %d, was "
                "expected %d\n",
                get_current_direction(g, x, y), dir);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  dir = E;
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece_current_direction(g, x, y, dir);
      if (get_current_direction(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_direction : Direction is %d, was "
                "expected %d\n",
                get_current_direction(g, x, y), dir);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  dir = S;
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece_current_direction(g, x, y, dir);
      if (get_current_direction(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_direction : Direction is %d, was "
                "expected %d\n",
                get_current_direction(g, x, y), dir);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  dir = W;
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece_current_direction(g, x, y, dir);
      if (get_current_direction(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_direction : Direction is %d, was "
                "expected %d\n",
                get_current_direction(g, x, y), dir);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  delete_game(g);
  return EXIT_SUCCESS;
}

/**
 * @brief Changes the current direction and piece type out of bounds. Checks if
 * all pieces kept their original type, current direction and original
 * direction.
 */
static int test_set_piece_out_of_bounds() {
  uint16_t width = 3;
  uint16_t height = 3;
  const piece initial_pieces[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                  EMPTY, EMPTY, EMPTY, EMPTY};
  const direction initial_directions[] = {N, N, N, N, N, N, N, N, N};

  game board =
      new_game_ext(width, height, initial_pieces, initial_directions, false);

  for (piece new_piece = LEAF; new_piece < NB_PIECE_TYPE; new_piece++) {
    for (direction new_direction = E; new_direction < NB_DIR; new_direction++) {
      set_piece(board, width,
                height,  // 'width' and 'height' are always out of bounds
                new_piece, new_direction);
      if (!compare_game_and_pieces_array(board, initial_pieces)) {
        FPRINTF(stderr,
                "Error: test_set_piece_out_of_bound, setting a piece out of "
                "bound changed a piece.\n");
        delete_game(board);
        return EXIT_FAILURE;
      }
      if (!compare_game_and_directions_array(board, initial_directions)) {
        FPRINTF(stderr,
                "Error: test_set_piece_out_of_bound, setting a piece out of "
                "bound changed the direction of a piece.\n");
        delete_game(board);
        return EXIT_FAILURE;
      }
    }
  }

  // We ensure a set_piece out of bounds didn't change the default direction
  restart_game(board);
  if (!compare_game_and_directions_array(board, initial_directions)) {
    FPRINTF(stderr,
            "Error: test_set_piece_out_of_bound, setting a piece out of "
            "bound changed the default direction of a piece.\n");
    delete_game(board);
    return EXIT_FAILURE;
  }

  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_shuffle_direction() {
  uint32_t seed = 0;
  srand(seed);

  piece default_pieces[] = {
      LEAF,   LEAF,    LEAF,    CROSS,   LEAF,  TEE,     TEE,   TEE,    CROSS,
      TEE,    SEGMENT, SEGMENT, SEGMENT, CROSS, SEGMENT, CROSS, CORNER, CORNER,
      CORNER, CORNER,  CROSS,   EMPTY,   EMPTY, EMPTY,   EMPTY};
  direction default_dirs[] = {N, S, W, E, N, N, S, W, E, N, N, S, W,
                              E, N, N, S, W, E, N, N, S, W, E, N};

  direction dTab[] = {N, S, W, E, N, N, S, W, E, N, N, S, W,
                      E, N, N, S, W, E, N, N, S, W, E, N};

  game g = new_game(default_pieces, default_dirs);
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);

  uint32_t same = 0;

  uint32_t roll_amount = 500;
  for (uint32_t i = 0; i < roll_amount; i++) {
    shuffle_direction(g);
    for (uint16_t y = 0; y < height; y++) {
      for (uint16_t x = 0; x < width; x++) {
        if (dTab[y * width + x] == get_current_direction(g, x, y)) {
          same++;
        }
        dTab[y * width + x] = get_current_direction(g, x, y);
      }
    }
  }
  uint32_t percentage = (same * 100) / (roll_amount * (height * width));
  uint32_t error_margin = 5;
  if (percentage > (100 / NB_DIR) - error_margin ||
      percentage < (100 / NB_DIR) + error_margin) {
    delete_game(g);
    return (EXIT_SUCCESS);
  } else {
    FPRINTF(stderr,
            "With %u of similarity, it's probably not a perfect random\n",
            percentage);
    delete_game(g);
    return (EXIT_FAILURE);
  }
}

/**
 * @brief Prints out the usage of this file.
 */
static void usage(char* program_name) {
  FPRINTF(stderr, "Usage: %s <testname>\n", program_name);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
  if (argc == 1) usage(argv[0]);

  PRINTF("=> RUN TEST \"%s\"\n", argv[1]);

  int status;
  if (strcmp("copy_game_valid", argv[1]) == 0)
    status = test_copy_game_valid();
  else if (strcmp("copy_game_null_game", argv[1]) == 0)
    status = test_copy_game_null_game();
  else if (strcmp("delete_game_valid_copy", argv[1]) == 0)
    status = test_delete_game_valid_copy();
  else if (strcmp("delete_game_valid_empty", argv[1]) == 0)
    status = test_delete_game_valid_empty();
  else if (strcmp("delete_game_valid_empty_ext", argv[1]) == 0)
    status = test_delete_game_valid_empty_ext();
  else if (strcmp("delete_game_valid_normal", argv[1]) == 0)
    status = test_delete_game_valid_normal();
  else if (strcmp("delete_game_valid_normal_ext", argv[1]) == 0)
    status = test_delete_game_valid_normal_ext();
  else if (strcmp("delete_game_null_game", argv[1]) == 0)
    status = test_delete_game_null_game();
  else if (strcmp("game_height_valid", argv[1]) == 0)
    status = test_game_height_valid();
  else if (strcmp("game_height_null_game", argv[1]) == 0)
    status = test_game_height_null_game();
  else if (strcmp("game_width_valid", argv[1]) == 0)
    status = test_game_width_valid();
  else if (strcmp("game_width_null_game", argv[1]) == 0)
    status = test_game_width_null_game();
  else if (strcmp("get_current_direction_valid", argv[1]) == 0)
    status = test_get_current_direction_valid();
  else if (strcmp("get_current_direction_null_game", argv[1]) == 0)
    status = test_get_current_direction_null_game();
  else if (strcmp("get_current_direction_out_of_bounds", argv[1]) == 0)
    status = test_get_current_direction_out_of_bounds();
  else if (strcmp("get_piece_valid", argv[1]) == 0)
    status = test_get_piece_valid();
  else if (strcmp("get_piece_null_game", argv[1]) == 0)
    status = test_get_piece_null_game();
  else if (strcmp("get_piece_out_of_bounds", argv[1]) == 0)
    status = test_get_piece_out_of_bounds();
  else if (strcmp("is_edge_EMPTY", argv[1]) == 0)
    status = test_is_edge_EMPTY();
  else if (strcmp("is_edge_LEAF", argv[1]) == 0)
    status = test_is_edge_LEAF();
  else if (strcmp("is_edge_SEGMENT", argv[1]) == 0)
    status = test_is_edge_SEGMENT();
  else if (strcmp("is_edge_CORNER", argv[1]) == 0)
    status = test_is_edge_CORNER();
  else if (strcmp("is_edge_TEE", argv[1]) == 0)
    status = test_is_edge_TEE();
  else if (strcmp("is_edge_CROSS", argv[1]) == 0)
    status = test_is_edge_CROSS();
  else if (strcmp("is_edge_coordinates_EMPTY", argv[1]) == 0)
    status = test_is_edge_coordinates_EMPTY();
  else if (strcmp("is_edge_coordinates_LEAF", argv[1]) == 0)
    status = test_is_edge_coordinates_LEAF();
  else if (strcmp("is_edge_coordinates_SEGMENT", argv[1]) == 0)
    status = test_is_edge_coordinates_SEGMENT();
  else if (strcmp("is_edge_coordinates_CORNER", argv[1]) == 0)
    status = test_is_edge_coordinates_CORNER();
  else if (strcmp("is_edge_coordinates_TEE", argv[1]) == 0)
    status = test_is_edge_coordinates_TEE();
  else if (strcmp("is_edge_coordinates_CROSS", argv[1]) == 0)
    status = test_is_edge_coordinates_CROSS();
  else if (strcmp("is_game_over_true", argv[1]) == 0)
    status = test_is_game_over_true();
  else if (strcmp("is_game_over_true_wrapped", argv[1]) == 0)
    status = test_is_game_over_true_wrapped();
  else if (strcmp("is_game_over_false", argv[1]) == 0)
    status = test_is_game_over_false();
  else if (strcmp("is_game_over_false_wrapped", argv[1]) == 0)
    status = test_is_game_over_false_wrapped();
  else if (strcmp("is_game_over_empty_game", argv[1]) == 0)
    status = test_is_game_over_empty_game();
  else if (strcmp("is_game_over_null_game", argv[1]) == 0)
    status = test_is_game_over_null_game();
  else if (strcmp("is_wrapping_true", argv[1]) == 0)
    status = test_is_wrapping_true();
  else if (strcmp("is_wrapping_false", argv[1]) == 0)
    status = test_is_wrapping_false();
  else if (strcmp("is_wrapping_null_game", argv[1]) == 0)
    status = test_is_wrapping_null_game();
  else if (strcmp("new_game", argv[1]) == 0)
    status = test_new_game();
  else if (strcmp("new_game_empty", argv[1]) == 0)
    status = test_new_game_empty();
  else if (strcmp("new_game_empty_ext", argv[1]) == 0)
    status = test_new_game_empty_ext();
  else if (strcmp("new_game_ext", argv[1]) == 0)
    status = test_new_game_ext();
  else if (strcmp("opposite_direction_N", argv[1]) == 0)
    status = test_opposite_direction_N();
  else if (strcmp("opposite_direction_E", argv[1]) == 0)
    status = test_opposite_direction_E();
  else if (strcmp("opposite_direction_S", argv[1]) == 0)
    status = test_opposite_direction_S();
  else if (strcmp("opposite_direction_W", argv[1]) == 0)
    status = test_opposite_direction_W();
  else if (strcmp("opposite_direction_invalid", argv[1]) == 0)
    status = test_opposite_direction_invalid();
  else if (strcmp("restart_game", argv[1]) == 0)
    status = test_restart_game();
  else if (strcmp("rotate_piece", argv[1]) == 0)
    status = test_rotate_piece();
  else if (strcmp("rotate_piece_one", argv[1]) == 0)
    status = test_rotate_piece_one();
  else if (strcmp("set_piece", argv[1]) == 0)
    status = test_set_piece();
  else if (strcmp("set_piece_current_direction", argv[1]) == 0)
    status = test_set_piece_current_dir();
  else if (strcmp("set_piece_out_of_bounds", argv[1]) == 0)
    status = test_set_piece_out_of_bounds();
  else if (strcmp("shuffle_direction", argv[1]) == 0)
    status = test_shuffle_direction();
  else {
    FPRINTF(stderr, "Error: test %s not found!\n", argv[1]);
    return EXIT_FAILURE;
  }

  if (status != EXIT_SUCCESS)
    PRINTF("FAILURE (status %d)\n", status);
  else
    PRINTF("SUCCESS (status %d)\n", status);
  return status;
}
