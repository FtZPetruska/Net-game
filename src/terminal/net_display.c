#include "draw_game.h"
#include "game.h"
#include "game_io.h"

#define BORDER_CHAR "█"

int main(int argc, char** argv) {
  if (argc != 2) {
    FPRINTF(stderr, "usage : ./net_display savegame\n");
    return EXIT_FAILURE;
  }
  game board = load_game(argv[1]);
  if (!board) {
    FPRINTF(stderr, "Error: net_display, unable to load the game.\n");
    return EXIT_FAILURE;
  }
  draw_game(board);
  delete_game(board);

  return EXIT_SUCCESS;
}
