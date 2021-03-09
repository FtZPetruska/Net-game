#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw_game.h"
#include "game.h"
#include "game_io.h"

#define BORDER_CHAR "█"

int main(int argc, char** argv) {
  if (argc != 2) {
    FPRINTF(stderr, "usage : ./net_display savegame\n");
    return EXIT_FAILURE;
  }
  game g = load_game(argv[1]);
  assert(g);
  draw_game(g);
  delete_game(g);

  return EXIT_SUCCESS;
}
