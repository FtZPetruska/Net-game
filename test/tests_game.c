#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

#define DEFAULT_SIZE 5

/* ********** TEST SET PIECE ********** */

// Tests if set_piece changes the initial directions of a game by changing all
// the pieces, then restarting the game, and comparing the array with the
// initial directions with the current game

int test_set_piece() {
  piece default_pieces[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                            EMPTY, EMPTY, EMPTY, EMPTY};
  direction default_dirs[] = {S, S, S, S, S, S, S, S, S, S, S, S, S,
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

/* ********** TEST GAME WIDTH ********** */

// Test if game_width returns the correct width of a standard game
// (DEFAULT_SIZE) and of an extended game (variable width)

int test_game_width() {
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

/* ********** TEST SET PIECE CURRENT DIR ********** */

// Test if set_piece_current_dir changes the cell's direction to the correct one
// by changing and checking the direction of every cell on the board for all 4
// directions

int test_set_piece_current_dir() {
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

/* ********** TEST OPPOSITE DIR ********** */

// Tests if opposite_dire returns the correct direction for every possible
// direction

int test_opposite_dir() {
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

/* ********** TEST DELETE GAME ********** */

// Tests if delete_game correctly frees a standard game and an extended game,
// expect external error such as double free corruption or segmentation fault in
// case of error

/*void sigsegv_handler(int sigsegv){
    exit(EXIT_SUCCESS);
}*/

int test_delete_game() {
  game g = new_game_empty();
  delete_game(g);
  game g_ext = new_game_empty_ext(10, 10, false);
  delete_game(g_ext);
  /*signal(SIGSEGV,sigsegv_handler); //Met en place une redirection temporaire
  en cas de Segmentation fault get_piece(g,0,0);
  //FPRINTF(stderr,"Error with Delete_game : Game wasn't freed properly,
  expected SegFault\n");*/
  return EXIT_SUCCESS;
}

/* ********** TEST RESTART GAME ********** */

// Tests if restart_game changes the initial dirrections and pieces of a freshly
// created game by shuffling it, restarting it and compare the result to the
// arrays of initial pieces and directions

int test_restart_game() {
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

/* ********** TEST new_game_empty ********** */

// Test if new_game_empty_ext creates a game properly by sending out a valid
// pointer, placing the correct width and height, and filling the grid with
// EMPTY pieces for both values of wrapping

int test_new_game_empty_ext() {
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

/* ********** TEST NEW GAME ********** */

// Test if new_game_ext creates a game properly by sending out a valid
// pointer, placing the correct width and height, and filling the grid with
// given pieces in a given direction for both values of wrapping

int test_new_game_ext() {
  piece default_pieces[] = {LEAF, CORNER, CORNER, CORNER, SEGMENT, CORNER, LEAF, CORNER, SEGMENT};
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

/* ********** TEST NEW GAME ********** */
/* test if new_game correclty generate the game object.
 */
int test_new_game() {
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
        FPRINTF(stderr,
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

/* ********** TEST GAME HEIGHT ********** */
/* test if game_height return the height of the game and not the age of the
 * captain
 */
int test_game_height() {
  game g = new_game_empty();
  if (game_height(g) != DEFAULT_SIZE) {
    FPRINTF(stderr,
            "Error: function int game_height(cgame game) is not working "
            "correctly or height is not at default size\n");
    delete_game(g);
    return EXIT_FAILURE;
  }
  delete_game(g);
  return EXIT_SUCCESS;
}

/* ********** TEST ROTATE PIECE ********** */
/* test if rotate_piece rotate correctly pieces
 */
int test_rotate_piece() {
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

/* ********** TEST IS EDGE ********** */
/* test if is_edge return a boolean to know if a given piece type can connect
 * itself in a given direction
 */
int test_is_edge() {
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

/* ********** TEST GET PIECE ********** */
/* test get_piece
 */
int test_get_piece() {
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

/* ********** TEST IS GAME OVER ********** */
/* test on a few examples if is_game_over correclty detect a finished game
 * board.
 */
int test_is_game_over() {
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

  piece default_pieces4[] = {LEAF, CORNER, CORNER, CORNER, SEGMENT, CORNER, LEAF, CORNER, SEGMENT};
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

/*
 *   this file implement test function for all function defined in game.h,
 *   it try if they work exactly like in their description
 *
 */

/* ********** TEST new_game_empty ********** */
/* test if on creation of a new game empty, the returned pointer is valid, the
 * game has good direction, and if it is empty.
 */
int test_new_game_empty() {
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
        FPRINTF(stderr,
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

/* ********** TEST shuffle_direction ********** */
/* By Generating many game boards, that test check if the shuffle_direction sfunction
 * shuffle correctly pieces. After shufle, a piece has 25% of chance to have
 * same position as before.
 */
int test_shuffle_direction() {
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

  /*
  double proba = (double)same / ((double)roll_amount * (height * width));

  double interval = 0.05;
  if (proba > (1.0 / (double)NB_DIR) - interval &&
      proba < (1.0 / (double)NB_DIR) + interval) {
    delete_game(g);
    return (EXIT_SUCCESS);
  } else {
    FPRINTF(stderr,
            "With %lf of similarity, it's probably not a perfect random\n",
            proba);
    delete_game(g);
    return (EXIT_FAILURE);
  } */
}

/* ********** TEST rotate_piece_one ********** */
/* That test check if the function to rotate one time correclty rotate in the
 * good direction, at the good place and one one time.
 */
int test_rotate_piece_one() {
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

/* ********** TEST is_edge_coordinates ********** */
/* This test check if the function is_edge_coordinates work correctly, that test
 * know which answer is_edge_coordinate must return depending of the direction
 * and type of the piece.
 */
int test_is_edge_coordinates() {
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

/* ********** TEST copy_game ********** */
/* That test check if the copy function copy correclty the original board, and
 * check if the new pointer is really a new board and not the same reference.
 */
int test_copy_game() {
  piece default_pieces[] = {LEAF,    LEAF,    LEAF,    LEAF,    LEAF,
                            TEE,     TEE,     TEE,     TEE,     TEE,
                            SEGMENT, SEGMENT, SEGMENT, SEGMENT, SEGMENT,
                            CORNER,  CORNER,  CORNER,  CORNER,  CORNER,
                            EMPTY,   EMPTY,   EMPTY,   EMPTY,   EMPTY};
  direction default_dirs[] = {N, S, W, E, N, N, S, W, E, N, N, S, W,
                              E, N, N, S, W, E, N, N, S, W, E, N};

  game board = new_game(default_pieces, default_dirs);

  game copy = copy_game((cgame)board);

  if (!board) {
    FPRINTF(stderr, "Board has been freed!\n");
    return EXIT_FAILURE;
  }

  if (game_height(board) != game_height(copy) ||
      game_width(board) != game_width(copy)) {
    delete_game(board);
    delete_game(copy);
    FPRINTF(stderr, "copy problem, with size\n");
    return EXIT_FAILURE;
  }

  for (uint16_t row = 0; row < game_height(board); row--) {
    for (uint16_t col = 0; col < game_width(board); col++) {
      direction dir = get_current_dir(board, col, row);
      piece p = get_piece(board, col, row);
      direction cDir = get_current_dir(copy, col, row);
      piece cP = get_piece(copy, col, row);

      if (dir != cDir || p != cP) {
        delete_game(board);
        delete_game(copy);
        FPRINTF(stderr, "copy problem, with dir or piece\n");
        return EXIT_FAILURE;
      }
    }
  }

  for (uint16_t row = 0; row < game_height(board); row--) {
    for (uint16_t col = 0; col < game_width(board); col++) {
      direction cDir2 = get_current_dir(copy, col, row);
      piece cP2 = get_piece(copy, col, row);

      if (get_piece(board, col, row) == EMPTY) {
        set_piece(board, col, row, TEE, get_current_dir(board, col, row));

      } else {
        set_piece(board, col, row, EMPTY, get_current_dir(board, col, row));
      }

      rotate_piece_one(board, col, row);

      direction cDir = get_current_dir(copy, col, row);
      piece cP = get_piece(copy, col, row);

      if (cDir2 != cDir || cP2 != cP) {
        delete_game(board);
        delete_game(copy);
        FPRINTF(stderr,
                "copy problem, the copy seems to copy just the pointer\n");
        return EXIT_FAILURE;
      }
    }
  }

  delete_game(board);
  delete_game(copy);
  return EXIT_SUCCESS;
}

/* ********** TEST get_current_dir ********** */
/* test if get_current_dir correctly get dir of a given cell.
 */
int test_get_current_dir() {
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
  direction values[width * height];

  for (uint16_t row = 0; row < height; row++) {
    for (uint16_t col = 0; col < width; col++) {
      values[row * width + col] = get_current_dir(board, col, row);
    }
  }

  for (uint16_t i = 0; i < width * height; i++) {
    if (default_dirs[i] != values[i]) {
      delete_game(board);
      FPRINTF(stderr, "problem to get curretn direction\n");
      return EXIT_FAILURE;
    }
  }

  delete_game(board);
  return EXIT_SUCCESS;
}

/* ********** TEST is_wrapping ********** */
/* test if that simple boolean getter work
 */
int test_is_wrapping() {
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

void usage(char* program_name) {
  FPRINTF(stderr, "Usage: %s <testname>\n", program_name);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
  if (argc == 1) usage(argv[0]);

  PRINTF("=> RUN TEST \"%s\"\n", argv[1]);

  int status;
  if (strcmp("new_game", argv[1]) == 0)
    status = test_new_game();
  else if (strcmp("game_height", argv[1]) == 0)
    status = test_game_height();
  else if (strcmp("rotate_piece", argv[1]) == 0)
    status = test_rotate_piece();
  else if (strcmp("is_edge", argv[1]) == 0)
    status = test_is_edge();
  else if (strcmp("get_piece", argv[1]) == 0)
    status = test_get_piece();
  else if (strcmp("is_game_over", argv[1]) == 0)
    status = test_is_game_over();
  else if (strcmp("new_game_empty", argv[1]) == 0)
    status = test_new_game_empty();
  else if (strcmp("shuffle_direction", argv[1]) == 0)
    status = test_shuffle_direction();
  else if (strcmp("rotate_piece_one", argv[1]) == 0)
    status = test_rotate_piece_one();
  else if (strcmp("is_edge_coordinates", argv[1]) == 0)
    status = test_is_edge_coordinates();
  else if (strcmp("copy_game", argv[1]) == 0)
    status = test_copy_game();
  else if (strcmp("get_current_dir", argv[1]) == 0)
    status = test_get_current_dir();
  else if (strcmp("is_wrapping", argv[1]) == 0)
    status = test_is_wrapping();
  else if (strcmp("set_piece", argv[1]) == 0)
    status = test_set_piece();
  else if (strcmp("game_width", argv[1]) == 0)
    status = test_game_width();
  else if (strcmp("set_piece_current_dir", argv[1]) == 0)
    status = test_set_piece_current_dir();
  else if (strcmp("opposite_dir", argv[1]) == 0)
    status = test_opposite_dir();
  else if (strcmp("delete_game", argv[1]) == 0)
    status = test_delete_game();
  else if (strcmp("restart_game", argv[1]) == 0)
    status = test_restart_game();
  else if (strcmp("new_game_empty_ext", argv[1]) == 0)
    status = test_new_game_empty_ext();
  else if (strcmp("new_game_ext", argv[1]) == 0)
    status = test_new_game_ext();
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
