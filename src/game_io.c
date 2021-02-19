#include "game_io.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

//--------------------------------------------------------------------------------------
//                                Static functions
static direction charToDir(char c);
static char dirToChar(direction d);
static piece charToPiece(char c);
static char pieceToChar(piece p);

//--------------------------------------------------------------------------------------
//                                 Prototyped functions
//                 NB: headers documentation for this functions are in the .h of
//                 this file.

game load_game(char* filename) {
  if (!filename) {
    fprintf(stderr, "Invalid file name! \n");
    return NULL;
  }

  FILE* gamefile = fopen(filename, "r");
  if (!gamefile) {
    fprintf(stderr, "Cannot open file! \n");
    return NULL;
  }

  int width, height;
  char wrapchar;
  if (!fscanf(gamefile, "%d %d %c ", &width, &height, &wrapchar)) {
    fprintf(stderr, "Error while reading the first line! \n");
    fclose(gamefile);
    return NULL;
  }
  if (width <= 0 || height <= 0) {
    fprintf(stderr, "Invalid width and height! \n");
    fclose(gamefile);
    return NULL;
  }

  if (wrapchar != 'N' && wrapchar != 'S') {
    fprintf(stderr, "Invalid wrapping value! \n");
    fclose(gamefile);
    return NULL;
  }

  game g = new_game_empty_ext(width, height, wrapchar == 'S');

  if (!g) {
    fprintf(stderr, "Not enough memory!\n");
    fclose(gamefile);
    return NULL;
  }

  char piecechar, dirchar;
  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      if (!fscanf(gamefile, "%c%c ", &piecechar, &dirchar)) {
        fprintf(stderr, "Error while reading the game file!\n");
        delete_game(g);
        fclose(gamefile);
        return NULL;
      }
      set_piece(g, x, y, charToPiece(piecechar), charToDir(dirchar));
    }
  }

  return g;
}

void save_game(cgame g, char* filename) {
  assert(g);
  assert(filename);
  FILE* f = fopen(filename, "w");
  assert(f);
  fprintf(f, "%d %d %c\n", game_width(g), game_height(g),
          is_wrapping(g) ? 'S' : 'N');

  for (int y = game_height(g) - 1; y >= 0; y--) {
    for (int x = 0; x < game_width(g) - 1; x++) {
      fprintf(f, "%c%c ", pieceToChar(get_piece(g, x, y)),
              dirToChar(get_current_dir(g, x, y)));
    }
    fprintf(f, "%c%c\n", pieceToChar(get_piece(g, game_width(g) - 1, y)),
            dirToChar(get_current_dir(g, game_width(g) - 1, y)));
  }
  fclose(f);
}

//--------------------------------------------------------------------------------------
//                                Static functions bodies

static char dirTab[] = {'N', 'E', 'S', 'W'};

static char pieceTab[] = {'L', 'S', 'C', 'T', 'F'};

static direction charToDir(char c) {
  for (int i = 0; i < NB_DIR; i++)
    if (c == dirTab[i]) return i;
  return N;
}

static char dirToChar(direction d) { return dirTab[d]; }

static piece charToPiece(char c) {
  for (int i = 0; i < NB_PIECE_TYPE; i++)
    if (c == pieceTab[i]) return i;
  return EMPTY;
}

static char pieceToChar(piece p) { return pieceTab[p]; }
