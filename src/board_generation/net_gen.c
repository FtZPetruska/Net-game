#include <time.h>

#include "game.h"
#include "game_io.h"
#include "game_rand.h"

void usage();

int main(int argc, char* argv[]) {
  srand((uint32_t)time(0));  // Turns on the random machine

  // Tests if there is a correct amount of arguments
  if (argc < 4 || 6 < argc) {
    usage();
    return EXIT_FAILURE;
  }

  game g;

  // Sets up all variables
  bool wrap = false;
  bool forbidCross = false;
  int fnameOffset = 0;

  if (argc >= 5) {
    wrap = strcmp("S", argv[3]);
    fnameOffset++;
  }

  if (argc == 6) {
    forbidCross = (atoi(argv[4]) == 3);
    fnameOffset++;
  }

  // Generates the game
  uint16_t width = (uint16_t)atoi(argv[1]);
  uint16_t height = (uint16_t)atoi(argv[2]);
  g = random_game_ext(width, height, wrap, !forbidCross);

  if (!g) {
    FPRINTF(stderr, "Error when generating a new game\n");
    usage();
    return EXIT_FAILURE;
  }

  // Save and delete the game
  save_game(g, argv[3 + fnameOffset]);

  delete_game(g);

  return EXIT_SUCCESS;
}

void usage() {
  FPRINTF(stderr,
          "./net_gen <width> <height> [S|N] [3|4] <filename>\n[S|N] [3|4] are "
          "optional, default : N 4\n");
}
