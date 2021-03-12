#include "game_io.h"

#include "game.h"

game load_game(char* filename) {
  if (!filename) {
    FPRINTF(stderr, "Error: load_game, filename pointer is NULL.\n");
    return NULL;
  }

  FILE* stream;
  FOPEN(stream, filename, "r");
  if (!stream) {
    FPRINTF(stderr, "Error: load_game, unable to open the savefile.\n");
    return NULL;
  }

  uint16_t width, height;
  uint32_t wrap;
#ifdef _WIN32
  if (!FSCANF(stream, "%hu %hu %d ", &width, &height, &wrap)) {
#else
  if (!FSCANF(stream, "%hu %hu %d ", &width, &height, &wrap)) {
#endif
    FPRINTF(stderr,
            "Error: load_game, unable to read header of the savefile.\n");
    FCLOSE(stream);
    return NULL;
  }
  if (width < MIN_GAME_WIDTH || MAX_GAME_WIDTH < width) {
    FPRINTF(stderr, "Error: load_game, the saved game has an invalid width.\n");
    FCLOSE(stream);
    return NULL;
  }

  if (height < MIN_GAME_HEIGHT || MAX_GAME_HEIGHT < height) {
    FPRINTF(stderr,
            "Error: load_game, the saved game has an invalid height.\n");
    FCLOSE(stream);
    return NULL;
  }

  game board = new_game_empty_ext(width, height, wrap == true);

  if (!board) {
    FPRINTF(stderr, "Error: load_game, couldn't create a game object.\n");
    FCLOSE(stream);
    return NULL;
  }

  piece current_piece;
  direction current_direction;
  for (uint16_t y = height; y-- > 0;) {
    for (uint16_t x = 0; x < width; x++) {
#ifdef _WIN32
      if (!FSCANF(stream, "%d;%d ", &current_piece, &current_direction)) {
#else
      if (!FSCANF(stream, "%d;%d ", &current_piece, &current_direction)) {
#endif

        FPRINTF(stderr, "Error while reading the game file!\n");
        delete_game(board);
        FCLOSE(stream);
        return NULL;
      }
      set_piece(board, x, y, current_piece, current_direction);
    }
  }

  return board;
}

void save_game(cgame board, char* filename) {
  if (!board) {
    FPRINTF(stderr, "Error: save_game, game pointer is NULL.\n");
    return;
  }

  if (!filename) {
    FPRINTF(stderr, "Error: save_game, filename pointer is NULL.\n");
    return;
  }

  FILE* stream;
  FOPEN(stream, filename, "w");

  if (!stream) {
    FPRINTF(stderr, "Error: save_game, unable to open the destination file.\n");
    return;
  }

  uint16_t width = game_width(board);
  uint16_t height = game_height(board);
  FPRINTF(stream, "%hu %hu %d\n", width, height,
          is_wrapping(board) ? true : false);

  for (uint16_t y = height; y-- > 0;) {
    for (uint16_t x = 0; x < width - 1; x++) {
      FPRINTF(stream, "%d;%d ", get_piece(board, x, y),
              get_current_dir(board, x, y));
    }
    FPRINTF(stream, "%d;%d\n", get_piece(board, game_width(board) - 1, y),
            get_current_dir(board, game_width(board) - 1, y));
  }
  FCLOSE(stream);
}
