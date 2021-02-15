/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * Romain Pigret-Cadou | Pamphile Saltel | Pierre Wendling
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "game_io.h"
#include "game_rand.h"

#define DEFAULT_SIZE 5
#define BORDER_CHAR "█"

// Prototypes:
void drawGame(game board);
void drawHBorder();
void usage();

// MAIN:
int main(int argc, char* argv[]) {
  srand(time(0));  // Turns on the random machine

  // Default variables:
  int turn = 1;
  piece default_pieces[] = {LEAF,    TEE,    LEAF,    LEAF, LEAF, LEAF, TEE,
                            TEE,     CORNER, SEGMENT, LEAF, LEAF, TEE,  LEAF,
                            SEGMENT, TEE,    TEE,     TEE,  TEE,  TEE,  CORNER,
                            LEAF,    LEAF,   CORNER,  LEAF};
  direction default_dirs[] = {E, W, S, E, S, S, S, N, W, S, E, N, W,
                              W, E, S, W, N, E, E, W, N, W, N, S};

  printf("\n\n\n--- < Nouvelle partie: > ---\n\n");

  // Setup of the game
  game mainGame;

  // Loads a game file
  if (argc == 2) {
    mainGame = load_game(argv[1]);
    if (!mainGame) {
      fprintf(stderr, "Error when loading game file\n");
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
    mainGame =
        random_game_ext(atoi(argv[1]), atoi(argv[2]), wrap, !forbidCross);
    if (!mainGame) {
      fprintf(stderr, "Error when generating a new game\n");
      usage();
      return (EXIT_FAILURE);
    }
  } else

      // In case too many arguments were given
      if (argc >= 6) {
    printf("Too many arguments given\n");
    usage();
    return (EXIT_FAILURE);
  } else {
    // Load the default game
    mainGame = new_game_ext(DEFAULT_SIZE, DEFAULT_SIZE, default_pieces,
                            default_dirs, false);
  }
  // shuffle_dir(mainGame);

  // game:

  int x;  // Coordinates of the piece to turn
  int y;

  // Main loop:
  while (!is_game_over(mainGame)) {
    // Displays the board
    printf("\n   Turn #%d :\n\n", turn);
    drawGame(mainGame);

    // Fetch the move
    bool rightValue = false;
    while (!rightValue) {  // Stays in this loop as long as the user doesn't
                           // enter a correct value
      printf("   Enter your move (<x> <y>) : ");
      char c;
      int* adress[] = {&x, &y};
      for (int i = 0; i < 2;
           i++) {  // Security to make sure given coordinates are valid

        while (!scanf("%d", adress[i])) {
          do {
            c = getchar();
          } while (c != ' ' && c != '\n');
        }
      }

      if (x >= 0 && x < game_width(mainGame) && y >= 0 &&
          y < game_height(mainGame))
        rightValue = true;
    }

    // Apply the move
    rotate_piece_one(mainGame, x, y);

    printf("\n________________________________\n\n");
    turn++;
  }

  // End of the game
  turn--;  // Because the turn counter is incremented at the end of each turn,
           // we have to reduce it by one
  printf("\n\n________________________________\n");
  printf("\n   VICTORY ! in %d turns\n\n", turn);
  drawGame(mainGame);
  printf("________________________________\n\n\n");
  delete_game(mainGame);

  return EXIT_SUCCESS;
}

// board display:
void drawGame(game board) {
  drawHBorder(board);
  for (int row = game_height(board) - 1; row >= 0; row--) {
    printf("      %s", BORDER_CHAR);
    for (int col = 0; col < game_width(board); col++) {
      printf(" ");

      switch (get_piece(board, col,
                        row)) {  // Switch to link a piece and its direction to
                                 // the corresponding character
        case EMPTY:
          printf("?");
          break;
        case LEAF:
          switch (get_current_dir(board, col, row)) {
            case N:
              printf("^");
              break;
            case S:
              printf("v");
              break;
            case W:
              printf("<");
              break;
            case E:
              printf(">");
              break;
          }
          break;
        case SEGMENT:
          switch (get_current_dir(board, col, row)) {
            case N:
              printf("|");
              break;
            case S:
              printf("|");
              break;
            case W:
              printf("-");
              break;
            case E:
              printf("-");
              break;
          }
          break;
        case CORNER:
          switch (get_current_dir(board, col, row)) {
            case N:
              printf("└");
              break;
            case S:
              printf("┐");
              break;
            case W:
              printf("┘");
              break;
            case E:
              printf("┌");
              break;
          }
          break;
        case TEE:
          switch (get_current_dir(board, col, row)) {
            case N:
              printf("┴");
              break;
            case S:
              printf("┬");
              break;
            case W:
              printf("┤");
              break;
            case E:
              printf("├");
              break;
          }
          break;
        case CROSS:
          printf("+");
          break;
        default:
          printf("?");
      }
    }
    printf(" ");
    printf("%s", BORDER_CHAR);
    printf("\n");
  }
  drawHBorder(board);
  printf("\n");
}

// Simply draws the border of the game board
void drawHBorder(game g) {
  printf("      ");
  for (int i = 0; i < 2 * game_width(g) + 2 + 1; i++) {
    printf("%s", BORDER_CHAR);
  }
  printf("\n");
}

// Prints how to use ./net_solve with arguments in case they're wrong

void usage() {
  fprintf(stderr,
          "./net_text <filename>\n./net_text <width> <height> [S|N] [3|4]\n");
}