#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_io.h"

#define DEFAULT_SIZE 5
#define BORDER_CHAR "█"

// prototypes:
void drawGame(game board);
void drawHBorder();

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "usage : ./net_display savegame\n");
    return EXIT_FAILURE;
  }
  game g = load_game(argv[1]);
  assert(g);
  drawGame(g);
  delete_game(g);

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
                        row)) {  // complexe switch to link a specific piece
                                 // with specific dir to the good charactere.
        case EMPTY:
          printf(" ");
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

// useful for display:
void drawHBorder(game g) {
  printf("      ");
  for (int i = 0; i < 2 * game_width(g) + 2 + 1; i++) {
    printf("%s", BORDER_CHAR);
  }
  printf("\n");
}
