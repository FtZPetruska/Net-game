#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "game_io.h"
#include "game_rand.h"

void usage();

int main(int argc, char* argv[]) {
  srand(time(0));  // Turns on the random machine

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
  g = random_game_ext(atoi(argv[1]), atoi(argv[2]), wrap, !forbidCross);

  if (!g) {
    fprintf(stderr, "Error when generating a new game\n");
    usage();
    return EXIT_FAILURE;
  }

  // Save and delete the game
  save_game(g, argv[3 + fnameOffset]);

  delete_game(g);

  return EXIT_SUCCESS;
}

void usage() {
  fprintf(stderr,
          "./net_gen <width> <height> [S|N] [3|4] <filename>\n[S|N] [3|4] are "
          "optional, default : N 4\n");
}