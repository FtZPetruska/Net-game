#include "game_io.h"

#include <assert.h>
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
    FPRINTF(stderr, "Invalid file name! \n");
    return NULL;
  }

  FILE* gamefile = FOPEN(filename, "r");
  if (!gamefile) {
    FPRINTF(stderr, "Cannot open file! \n");
    return NULL;
  }

  uint16_t width, height;
  char wrapchar;
  if (!fscanf(gamefile, "%hu %hu %c ", &width, &height, &wrapchar)) {
    FPRINTF(stderr, "Error while reading the first line! \n");
    FCLOSE(gamefile);
    return NULL;
  }
  if (width <= 0 || height <= 0) {
    FPRINTF(stderr, "Invalid width and height! \n");
    FCLOSE(gamefile);
    return NULL;
  }

  if (wrapchar != 'N' && wrapchar != 'S') {
    FPRINTF(stderr, "Invalid wrapping value! \n");
    FCLOSE(gamefile);
    return NULL;
  }

  game g = new_game_empty_ext(width, height, wrapchar == 'S');

  if (!g) {
    FPRINTF(stderr, "Not enough memory!\n");
    FCLOSE(gamefile);
    return NULL;
  }

  char piecechar, dirchar;
  for (uint16_t y = height; y-- > 0;) {
    for (uint16_t x = 0; x < width; x++) {
      if (!FSCANF(gamefile, "%c%c ", &piecechar, &dirchar)) {
        FPRINTF(stderr, "Error while reading the game file!\n");
        delete_game(g);
        FCLOSE(gamefile);
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
  FILE* f = FOPEN(filename, "w");
  assert(f);
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  FPRINTF(f, "%hu %hu %c\n", width, height, is_wrapping(g) ? 'S' : 'N');

  for (uint16_t y = height; y-- > 0;) {
    for (uint16_t x = 0; x < width - 1; x++) {
      FPRINTF(f, "%c%c ", pieceToChar(get_piece(g, x, y)),
              dirToChar(get_current_dir(g, x, y)));
    }
    FPRINTF(f, "%c%c\n", pieceToChar(get_piece(g, game_width(g) - 1, y)),
            dirToChar(get_current_dir(g, game_width(g) - 1, y)));
  }
  fclose(f);
}

//--------------------------------------------------------------------------------------
//                                Static functions bodies

static const char dirTab[] = {'N', 'E', 'S', 'W'};

static const char pieceTab[] = {'L', 'S', 'C', 'T', 'F'};

static direction charToDir(char c) {
  for (direction i = N; i < NB_DIR; i++)
    if (c == dirTab[i]) return i;
  return N;
}

static char dirToChar(direction d) { return dirTab[d]; }

static piece charToPiece(char c) {
  for (piece i = EMPTY; i < NB_PIECE_TYPE; i++)
    if (c == pieceTab[i]) return i;
  return EMPTY;
}

static char pieceToChar(piece p) { return pieceTab[p]; }
