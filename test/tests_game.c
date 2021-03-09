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
      direction current_direction = get_current_dir(board, x, y);
      if (current_direction != expected_directions[x + width * y]) {
        FPRINTF(stderr,
                "Error: compare_game_and_directions_array, piece at (%hu,%hu) "
                "has direction "
                "%u while %u was expected.\n",
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

      direction current_source_direction = get_current_dir(source_board, x, y);
      direction current_copied_direction = get_current_dir(copied_board, x, y);
      if (current_source_direction != current_copied_direction) {
        FPRINTF(stderr,
                "Error: test_copy_game_valid, at (%hu,%hu) copied piece has "
                "direction %u while %u is the direction in source.\n",
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
 * @brief
 */
static int test_game_width() {
  game g = new_game_empty();
  if (game_width(g) == DEFAULT_SIZE) {
    delete_game(g);
  } else {
    FPRINTF(stderr,
            "Error with game_width : Width of standard game is %d, was "
            "expected %d\n",
            game_width(g), DEFAULT_SIZE);
    delete_game(g);
    return EXIT_FAILURE;
  }
  uint16_t width = 42;
  uint16_t height = 115;
  game g_ext = new_game_empty_ext(width, height, false);
  if (game_width(g_ext) == width && game_height(g_ext) == height) {
    delete_game(g_ext);
    return EXIT_SUCCESS;
  }
  FPRINTF(
      stderr,
      "Error with game_width : Width of extended game is %d, was expected %d\n",
      game_width(g_ext), width);
  return EXIT_FAILURE;
}

/**
 * @brief
 */
static int test_get_current_dir() {
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
  direction* values = (direction*)malloc(width * height * sizeof(direction));

  for (uint16_t row = 0; row < height; row++) {
    for (uint16_t col = 0; col < width; col++) {
      values[row * width + col] = get_current_dir(board, col, row);
    }
  }

  for (uint16_t i = 0; i < width * height; i++) {
    if (default_dirs[i] != values[i]) {
      delete_game(board);
      free(values);
      FPRINTF(stderr, "problem to get curretn direction\n");
      return EXIT_FAILURE;
    }
  }

  delete_game(board);
  free(values);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_get_piece() {
  game g = new_game_empty();
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      for (piece i = EMPTY; i < NB_PIECE_TYPE; i++) {
        set_piece(g, x, y, i, N);
        if (get_piece(g, x, y) != i) {
          FPRINTF(stderr,
                  "Error: function get_piece(game, %d, %d) returned a wrong "
                  "answer (%d instead of %d)\n",
                  x, y, get_piece(g, x, y), i);
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
static int test_is_edge() {
  for (direction i = 0; i < NB_DIR; i++) {
    for (direction j = 0; j < NB_DIR; j++) {
      if (is_edge(EMPTY, i, j) != false) {
        FPRINTF(stderr,
                "Error: function is_edge(EMPTY, %d, %d) returned a wrong "
                "answer (%d)\n",
                i, j, is_edge(EMPTY, i, j));
        return EXIT_FAILURE;
      }
      if (is_edge(LEAF, i, j) != (i == j)) {
        FPRINTF(stderr,
                "Error: function is_edge(LEAF, %d, %d) returned a wrong answer "
                "(%d)\n",
                i, j, is_edge(LEAF, i, j));
        return EXIT_FAILURE;
      }
      if (is_edge(SEGMENT, i, j) != (i % 2 == j % 2)) {
        FPRINTF(stderr,
                "Error: function is_edge(SEGMENT, %d, %d) returned a wrong "
                "answer (%d)\n",
                i, j, is_edge(SEGMENT, i, j));
        return EXIT_FAILURE;
      }
      if (is_edge(CORNER, i, j) != (i == j || (i + 1) % 4 == j)) {
        FPRINTF(stderr,
                "Error: function is_edge(CORNER, %d, %d) returned a wrong "
                "answer (%d)\n",
                i, j, is_edge(CORNER, i, j));
        return EXIT_FAILURE;
      }
      if (is_edge(TEE, i, j) !=
          (i == (j + 1) % 4 || i == j || (i + 1) % 4 == j)) {
        FPRINTF(stderr,
                "Error: function is_edge(TEE, %d, %d) returned a wrong answer "
                "(%d)\n",
                i, j, is_edge(TEE, i, j));
        return EXIT_FAILURE;
      }
      if (is_edge(CROSS, i, j) != true) {
        FPRINTF(stderr,
                "Error: function is_edge(CROSS, %d, %d) returned a wrong "
                "answer (%d)\n",
                i, j, is_edge(CROSS, i, j));
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_is_edge_coordinates() {
  piece default_pieces[] = {
      LEAF,   LEAF,    LEAF,    LEAF,    LEAF,    TEE,   TEE,    TEE,    TEE,
      TEE,    SEGMENT, SEGMENT, SEGMENT, SEGMENT, CROSS, CORNER, CORNER, CROSS,
      CORNER, CORNER,  EMPTY,   EMPTY,   EMPTY,   EMPTY, EMPTY};
  direction default_dirs[] = {N, S, W, E, N, N, S, W, E, N, N, S, W,
                              E, N, N, S, W, E, N, N, S, W, E, N};

  game board = new_game(default_pieces, default_dirs);
  uint16_t width = game_width(board);
  uint16_t height = game_height(board);
  for (uint16_t row = 0; row < height; row--) {
    for (uint16_t col = 0; col < width; col++) {
      direction dir = get_current_dir(board, col, row);
      piece p = get_piece(board, col, row);

      switch (p) {
        case CROSS:
          if (!is_edge_coordinates(board, col, row, N) ||
              !is_edge_coordinates(board, col, row, S) ||
              !is_edge_coordinates(board, col, row, E) ||
              !is_edge_coordinates(board, col, row, W)) {
            delete_game(board);
            FPRINTF(stderr, "CROSS can connect in all directions\n");
            return EXIT_FAILURE;
          }
          break;

        case EMPTY:
          if (is_edge_coordinates(board, col, row, N) ||
              is_edge_coordinates(board, col, row, S) ||
              is_edge_coordinates(board, col, row, E) ||
              is_edge_coordinates(board, col, row, W)) {
            delete_game(board);
            FPRINTF(stderr, "EMPTY cannot have edge\n");
            return EXIT_FAILURE;
          }
          break;

        case LEAF:
          if ((is_edge_coordinates(board, col, row, N) && dir != N) ||
              (is_edge_coordinates(board, col, row, S) && dir != S) ||
              (is_edge_coordinates(board, col, row, E) && dir != E) ||
              (is_edge_coordinates(board, col, row, W) && dir != W)) {
            delete_game(board);
            FPRINTF(stderr, "LEAF edge problem (it say true)\n");
            return EXIT_FAILURE;
          }
          if ((!is_edge_coordinates(board, col, row, N) && dir == N) ||
              (!is_edge_coordinates(board, col, row, S) && dir == S) ||
              (!is_edge_coordinates(board, col, row, E) && dir == E) ||
              (!is_edge_coordinates(board, col, row, W) && dir == W)) {
            delete_game(board);
            FPRINTF(stderr, "LEAF edge problem (it say false)\n");
            return EXIT_FAILURE;
          }
          break;

        case CORNER:
          if ((is_edge_coordinates(board, col, row, N) && dir != N &&
               dir != W) ||
              (is_edge_coordinates(board, col, row, S) && dir != S &&
               dir != E) ||
              (is_edge_coordinates(board, col, row, E) && dir != E &&
               dir != N) ||
              (is_edge_coordinates(board, col, row, W) && dir != W &&
               dir != S)) {
            delete_game(board);
            FPRINTF(stderr, "CORNER edge problem (it say true)\n");
            return EXIT_FAILURE;
          }
          if ((!is_edge_coordinates(board, col, row, N) &&
               (dir == N || dir == W)) ||
              (!is_edge_coordinates(board, col, row, S) &&
               (dir == S || dir == E)) ||
              (!is_edge_coordinates(board, col, row, E) &&
               (dir == E || dir == N)) ||
              (!is_edge_coordinates(board, col, row, W) &&
               (dir == W || dir == S))) {
            delete_game(board);
            FPRINTF(stderr, "CORNER edge problem (it say false)\n");
            return EXIT_FAILURE;
          }
          break;

        case SEGMENT:
          if ((is_edge_coordinates(board, col, row, N) && dir != N &&
               dir != S) ||
              (is_edge_coordinates(board, col, row, S) && dir != N &&
               dir != S) ||
              (is_edge_coordinates(board, col, row, E) && dir != E &&
               dir != W) ||
              (is_edge_coordinates(board, col, row, W) && dir != E &&
               dir != W)) {
            delete_game(board);
            FPRINTF(stderr, "SEGMENT edge problem (it say true)\n");
            return EXIT_FAILURE;
          }
          if ((!is_edge_coordinates(board, col, row, N) &&
               (dir == N || dir == S)) ||
              (!is_edge_coordinates(board, col, row, S) &&
               (dir == N || dir == S)) ||
              (!is_edge_coordinates(board, col, row, E) &&
               (dir == E || dir == W)) ||
              (!is_edge_coordinates(board, col, row, W) &&
               (dir == E || dir == W))) {
            delete_game(board);
            FPRINTF(stderr, "SEGMENT edge problem (it say false)\n");
            return EXIT_FAILURE;
          }
          break;

        case TEE:
          if ((is_edge_coordinates(board, col, row, N) && dir == S) ||
              (is_edge_coordinates(board, col, row, S) && dir == N) ||
              (is_edge_coordinates(board, col, row, E) && dir == W) ||
              (is_edge_coordinates(board, col, row, W) && dir == E)) {
            delete_game(board);
            FPRINTF(stderr, "TEE edge problem (it say true)\n");
            return EXIT_FAILURE;
          }
          if ((!is_edge_coordinates(board, col, row, N) && dir != S) ||
              (!is_edge_coordinates(board, col, row, S) && dir != N) ||
              (!is_edge_coordinates(board, col, row, E) && dir != W) ||
              (!is_edge_coordinates(board, col, row, W) && dir != E)) {
            delete_game(board);
            FPRINTF(stderr, "TEE edge problem (it say false)\n");
            return EXIT_FAILURE;
          }
          break;
      }
    }
  }

  delete_game(board);
  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_is_game_over() {
  piece default_pieces[] = {LEAF,    TEE,    LEAF,    LEAF, LEAF, LEAF, TEE,
                            TEE,     CORNER, SEGMENT, LEAF, LEAF, TEE,  LEAF,
                            SEGMENT, TEE,    TEE,     TEE,  TEE,  TEE,  CORNER,
                            LEAF,    LEAF,   CORNER,  LEAF};
  direction solution_dirs[] = {E, N, W, N, N, E, S, N, S, N, N, N, E,
                               W, N, E, S, S, N, W, E, W, E, S, S};
  game g = new_game(default_pieces, solution_dirs);
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  direction dir;
  piece pice;

  if (!is_game_over(g)) {
    FPRINTF(stderr,
            "Error: function bool is_game_over(cgame g) returned \"false\" in "
            "a finished game\n");
    delete_game(g);
    return EXIT_FAILURE;
  }

  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      dir = get_current_dir(g, x, y);
      pice = get_piece(g, x, y);
      rotate_piece_one(g, x, y);
      if (is_game_over(g)) {
        FPRINTF(stderr,
                "Error: function bool is_game_over(cgame g) returned \"true\" "
                "in a not finished game\n");
        delete_game(g);
        return EXIT_FAILURE;
      }
      set_piece(g, x, y, (pice + 1) % 4, dir);
      if (is_game_over(g)) {
        FPRINTF(stderr,
                "Error: function bool is_game_over(cgame g) returned \"true\" "
                "in a not finished game\n");
        delete_game(g);
        return EXIT_FAILURE;
      }
      set_piece(g, x, y, pice, dir);
      if (!is_game_over(g)) {
        FPRINTF(stderr,
                "Error: function bool is_game_over(cgame g) returned \"false\" "
                "in a finished game\n");
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  delete_game(g);

  piece default_pieces2[] = {
      CORNER,  CORNER, LEAF,   LEAF, LEAF, CORNER,  CORNER, TEE, CORNER,
      SEGMENT, LEAF,   LEAF,   TEE,  LEAF, SEGMENT, TEE,    TEE, TEE,
      TEE,     TEE,    CORNER, LEAF, LEAF, CORNER,  LEAF};
  direction default_dirs[] = {N, W, N, N, N, E, S, E, S, N, N, N, E,
                              W, N, E, S, S, N, W, E, W, E, S, S};
  game g2 = new_game(default_pieces2, default_dirs);
  if (is_game_over(g2)) {
    FPRINTF(stderr,
            "Error: function bool is_game_over(cgame g) returned \"true\" in a "
            "finished game containing a disconnected loop\n");
    delete_game(g2);
    return EXIT_FAILURE;
  }
  delete_game(g2);

  piece default_pieces3[] = {
      CORNER,  TEE,  LEAF,   LEAF, LEAF, CORNER,  TEE, TEE, CORNER,
      SEGMENT, LEAF, LEAF,   TEE,  LEAF, SEGMENT, TEE, TEE, TEE,
      TEE,     TEE,  CORNER, LEAF, LEAF, CORNER,  LEAF};
  direction default_dirs2[] = {N, N, W, N, N, E, S, N, S, N, N, N, E,
                               W, N, E, S, S, N, W, E, W, E, S, S};
  game g3 = new_game(default_pieces3, default_dirs2);
  if (is_game_over(g3)) {
    FPRINTF(stderr,
            "Error: function bool is_game_over(cgame g) returned \"true\" in a "
            "finished game containing a loop\n");
    delete_game(g3);
    return EXIT_FAILURE;
  }
  delete_game(g3);

  piece default_pieces4[] = {LEAF,   CORNER, CORNER, CORNER, SEGMENT,
                             CORNER, LEAF,   CORNER, SEGMENT};
  direction default_dirs3[] = {N, E, S, E, E, W, E, W, N};
  game g4 = new_game_ext(3, 3, default_pieces4, default_dirs3, true);

  if (!is_game_over(g4)) {
    FPRINTF(stderr,
            "Error: function bool is_game_over(cgame g) returned \"false\" in "
            "a finished game (wrapping problem)\n");
    delete_game(g4);
    return EXIT_FAILURE;
  }

  delete_game(g4);

  return EXIT_SUCCESS;
}

/**
 * @brief
 */
static int test_is_wrapping() {
  piece default_pieces[] = {LEAF,    LEAF,    LEAF,    LEAF,    LEAF,
                            TEE,     TEE,     TEE,     TEE,     TEE,
                            SEGMENT, SEGMENT, SEGMENT, SEGMENT, SEGMENT,
                            CORNER,  CORNER,  CORNER,  CORNER,  CORNER,
                            EMPTY,   EMPTY,   EMPTY,   EMPTY,   EMPTY};
  direction default_dirs[] = {N, S, W, E, N, N, S, W, E, N, N, S, W,
                              E, N, N, S, W, E, N, N, S, W, E, N};
  game g1 = new_game_ext(DEFAULT_SIZE, DEFAULT_SIZE, default_pieces,
                         default_dirs, true);
  if (!is_wrapping(g1)) {
    delete_game(g1);
    FPRINTF(stderr, "is_wrapping say there is no wrap but there is !\n");
    return EXIT_FAILURE;
  }
  game g2 = new_game_ext(5, 5, default_pieces, default_dirs, false);
  if (is_wrapping(g2)) {
    delete_game(g2);
    delete_game(g1);
    FPRINTF(stderr, "is_wrapping say there is wrap but there is not !\n");
    return EXIT_FAILURE;
  }

  delete_game(g1);
  delete_game(g2);
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
      if (get_current_dir(g, i, j) != default_dirs[(j * width) + i]) {
        FPRINTF(stderr,
                "Error: piece's direction (%hu,%hu) is not corresponding! "
                "(should be %d, is %d)\n",
                i, j, default_dirs[(j * width) + i], get_current_dir(g, i, j));
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

    for (uint16_t row = 0; row < height; row--) {
      for (uint16_t col = 0; col < width; col++) {
        if (get_piece(g, col, row) != EMPTY) {
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

    for (uint16_t row = 0; row < height; row++) {
      for (uint16_t col = 0; col < width; col++) {
        if (get_piece(g, row, col) != default_pieces[(col * width) + row]) {
          FPRINTF(stderr,
                  "Error with new_game_ext : piece (%d,%d) is not "
                  "corresponding! (should be %d, is "
                  "%d)\n",
                  row, col, default_pieces[(col * width) + row],
                  get_piece(g, row, col));
          delete_game(g);
          return EXIT_FAILURE;
        }
        if (get_current_dir(g, row, col) != default_dirs[(col * width) + row]) {
          FPRINTF(stderr,
                  "Error with new_game_ext : piece's direction (%d,%d) is not "
                  "corresponding! "
                  "(should be %d, is %d)\n",
                  row, col, default_dirs[(col * width) + row],
                  get_current_dir(g, row, col));
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
 * @brief
 */
static int test_opposite_dir() {
  if (opposite_dir(N) != S) {
    FPRINTF(stderr,
            "Error with opposite_dir : opposite of %d is %d, was expected %d\n",
            0, opposite_dir(N), 2);
    return EXIT_FAILURE;
  }
  if (opposite_dir(E) != W) {
    FPRINTF(stderr,
            "Error with opposite_dir : opposite of %d is %d, was expected %d\n",
            1, opposite_dir(E), 3);
    return EXIT_FAILURE;
  }
  if (opposite_dir(S) != N) {
    FPRINTF(stderr,
            "Error with opposite_dir : opposite of %d is %d, was expected %d\n",
            2, opposite_dir(S), 0);
    return EXIT_FAILURE;
  }
  if (opposite_dir(W) != E) {
    FPRINTF(stderr,
            "Error with opposite_dir : opposite of %d is %d, was expected %d\n",
            3, opposite_dir(W), 1);
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
          get_current_dir(g, x, y) != default_dirs[x + y * DEFAULT_SIZE]) {
        FPRINTF(stderr,
                "Error with restart_game : after a restart the piece "
                "was %d and should be %d and the dir was %d and should be %d "
                "@(x=%d,y=%d)\n",
                get_piece(g, x, y), default_pieces[x + y * DEFAULT_SIZE],
                get_current_dir(g, x, y), default_dirs[x + y * DEFAULT_SIZE], x,
                y);
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
        dir = get_current_dir(g, x, y);
        rotate_piece(g, x, y, i);
        if (get_current_dir(g, x, y) != (dir + (uint32_t)i) % 4) {
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
  for (uint16_t row = 0; row < height; row++) {
    for (uint16_t col = 0; col < width; col++) {
      direction dir = get_current_dir(board, col, row);
      rotate_piece_one(board, col, row);
      switch (get_current_dir(board, col, row)) {
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
          get_current_dir(g, x, y) != default_dirs[x + y * DEFAULT_SIZE]) {
        FPRINTF(stderr,
                "Error when set_piece : after a restart the piece "
                "was %d and should be %d and the dir was %d and should be %d "
                "@(x=%d,y=%d)\n",
                get_piece(g, x, y), default_pieces[x + y * DEFAULT_SIZE],
                get_current_dir(g, x, y), default_dirs[x + y * DEFAULT_SIZE], x,
                y);
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
      set_piece_current_dir(g, x, y, dir);
      if (get_current_dir(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_dir : Direction is %d, was "
                "expected %d\n",
                get_current_dir(g, x, y), dir);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  dir = E;
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece_current_dir(g, x, y, dir);
      if (get_current_dir(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_dir : Direction is %d, was "
                "expected %d\n",
                get_current_dir(g, x, y), dir);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  dir = S;
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece_current_dir(g, x, y, dir);
      if (get_current_dir(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_dir : Direction is %d, was "
                "expected %d\n",
                get_current_dir(g, x, y), dir);
        delete_game(g);
        return EXIT_FAILURE;
      }
    }
  }
  dir = W;
  for (uint16_t x = 0; x < width; x++) {
    for (uint16_t y = 0; y < height; y++) {
      set_piece_current_dir(g, x, y, dir);
      if (get_current_dir(g, x, y) != dir) {
        FPRINTF(stderr,
                "Error with set_piece_current_dir : Direction is %d, was "
                "expected %d\n",
                get_current_dir(g, x, y), dir);
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
 *
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
        if (dTab[y * width + x] == get_current_dir(g, x, y)) {
          same++;
        }
        dTab[y * width + x] = get_current_dir(g, x, y);
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
  else if (strcmp("game_width", argv[1]) == 0)
    status = test_game_width();
  else if (strcmp("get_current_dir", argv[1]) == 0)
    status = test_get_current_dir();
  else if (strcmp("get_piece", argv[1]) == 0)
    status = test_get_piece();
  else if (strcmp("is_edge", argv[1]) == 0)
    status = test_is_edge();
  else if (strcmp("is_edge_coordinates", argv[1]) == 0)
    status = test_is_edge_coordinates();
  else if (strcmp("is_game_over", argv[1]) == 0)
    status = test_is_game_over();
  else if (strcmp("is_wrapping", argv[1]) == 0)
    status = test_is_wrapping();
  else if (strcmp("new_game", argv[1]) == 0)
    status = test_new_game();
  else if (strcmp("new_game_empty", argv[1]) == 0)
    status = test_new_game_empty();
  else if (strcmp("new_game_empty_ext", argv[1]) == 0)
    status = test_new_game_empty_ext();
  else if (strcmp("new_game_ext", argv[1]) == 0)
    status = test_new_game_ext();
  else if (strcmp("opposite_dir", argv[1]) == 0)
    status = test_opposite_dir();
  else if (strcmp("restart_game", argv[1]) == 0)
    status = test_restart_game();
  else if (strcmp("rotate_piece", argv[1]) == 0)
    status = test_rotate_piece();
  else if (strcmp("rotate_piece_one", argv[1]) == 0)
    status = test_rotate_piece_one();
  else if (strcmp("set_piece", argv[1]) == 0)
    status = test_set_piece();
  else if (strcmp("set_piece_current_dir", argv[1]) == 0)
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
