#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "draw_game.h"
#include "game.h"
#include "game_io.h"
#include "game_rand.h"

#define DEFAULT_SIZE 5

// Prototypes:
void usage();

// MAIN:
int main(int argc, char* argv[]) {
  srand((uint32_t)time(0));  // Turns on the random machine

  // Default variables:
  uint16_t turn = 1;
  piece default_pieces[] = {LEAF,    TEE,    LEAF,    LEAF, LEAF, LEAF, TEE,
                            TEE,     CORNER, SEGMENT, LEAF, LEAF, TEE,  LEAF,
                            SEGMENT, TEE,    TEE,     TEE,  TEE,  TEE,  CORNER,
                            LEAF,    LEAF,   CORNER,  LEAF};
  direction default_dirs[] = {E, W, S, E, S, S, S, N, W, S, E, N, W,
                              W, E, S, W, N, E, E, W, N, W, N, S};

  PRINTF("\n\n\n--- < Nouvelle partie: > ---\n\n");

  // Setup of the game
  game mainGame;

  // Loads a game file
  if (argc == 2) {
    mainGame = load_game(argv[1]);
    if (!mainGame) {
      FPRINTF(stderr, "Error when loading game file\n");
      usage();
      return (EXIT_FAILURE);
    }
  } else
      // Generates games depending on the number of arguments
      if (argc > 2 && argc < 6) {
    bool wrap = false;
    bool forbidCross = false;
    if (argc == 4) wrap = strcmp("S", argv[3]);
    if (argc == 5) forbidCross = (atoi(argv[4]) == 3);

    uint16_t width = (uint16_t)atoi(argv[1]);
    uint16_t height = (uint16_t)atoi(argv[2]);
    mainGame = random_game_ext(width, height, wrap, !forbidCross);
    if (!mainGame) {
      FPRINTF(stderr, "Error when generating a new game\n");
      usage();
      return (EXIT_FAILURE);
    }
  } else

      // In case too many arguments were given
      if (argc >= 6) {
    PRINTF("Too many arguments given\n");
    usage();
    return (EXIT_FAILURE);
  } else {
    // Load the default game
    mainGame = new_game_ext(DEFAULT_SIZE, DEFAULT_SIZE, default_pieces,
                            default_dirs, false);
  }

  // game:

  uint16_t x, y;

  // Main loop:
  while (!is_game_over(mainGame)) {
    // Displays the board
    PRINTF("\n   Turn #%d :\n\n", turn);
    draw_game(mainGame);

    // Fetch the move
    bool rightValue = false;
    while (!rightValue) {  // Stays in this loop as long as the user doesn't
                           // enter a correct value
      PRINTF("   Enter your move (<x> <y>) : ");
      char c;
      uint16_t* adress[] = {&x, &y};
      for (int i = 0; i < 2;
           i++) {  // Security to make sure given coordinates are valid

        while (!SCANF("%hu", adress[i])) {
          do {
            c = (char)getchar();
          } while (c != ' ' && c != '\n');
        }
      }

      if (x >= 0 && x < game_width(mainGame) && y >= 0 &&
          y < game_height(mainGame))
        rightValue = true;
    }

    // Apply the move
    rotate_piece_one(mainGame, x, y);

    PRINTF("\n________________________________\n\n");
    turn++;
  }

  // End of the game
  turn--;  // Because the turn counter is incremented at the end of each turn,
           // we have to reduce it by one
  PRINTF("\n\n________________________________\n");
  PRINTF("\n   VICTORY ! in %d turns\n\n", turn);
  draw_game(mainGame);
  PRINTF("________________________________\n\n\n");
  delete_game(mainGame);

  return EXIT_SUCCESS;
}

// Prints how to use ./net_solve with arguments in case they're wrong

void usage() {
  FPRINTF(stderr, 
      "./net_text <filename>\n./net_text <width> <height> [S|N] [3|4]\n");
}
