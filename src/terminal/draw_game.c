#include "draw_game.h"

static const char* BORDER_CHAR = "\u2588";
static const char* LEFT_SPACING = "      ";
static const char* PIECE_SPACING = " ";
static const char* PIECE_TABLE[NB_PIECE_TYPE + 1][NB_DIR] = {
    {"?", "?", "?", "?"}, {"^", ">", "v", "<"}, {"|", "-", "|", "-"},
    {"└", "┌", "┐", "┘"}, {"┴", "├", "┬", "┤"}, {"+", "+", "+", "+"}};

static void draw_border(uint16_t width) {
  PRINTF("%s", LEFT_SPACING);
  for (uint16_t i = 0; i < 2 * width + 3; i++) {
    PRINTF("%s", BORDER_CHAR);
  }
  PRINTF("%c", '\n');
}

static void draw_piece(piece current_piece, direction current_direction) {
  PRINTF("%s", PIECE_SPACING);
  if (current_piece < EMPTY || CROSS < current_piece) {
    current_piece = EMPTY;
  }
  if (current_direction < N || W < current_direction) {
    current_direction = N;
  }
  current_piece++;  // The first piece of the enum is at -1
  PRINTF("%s", PIECE_TABLE[current_piece][current_direction]);
}

void draw_game(game board) {
  if (!board) {
    FPRINTF(stderr, "Error: draw_game, game pointer is NULL.\n");
    return;
  }

  uint16_t width = game_width(board);
  uint16_t height = game_height(board);

  draw_border(width);
  for (uint16_t y = height; y-- > 0;) {
    PRINTF("%s%s", LEFT_SPACING, BORDER_CHAR);

    for (uint16_t x = 0; x < width; x++) {
      piece current_piece = get_piece(board, x, y);
      direction current_direction = get_current_direction(board, x, y);
      draw_piece(current_piece, current_direction);
    }

    PRINTF("%s%s%c", PIECE_SPACING, BORDER_CHAR, '\n');
  }
  draw_border(width);
  PRINTF("%c", '\n');
}
