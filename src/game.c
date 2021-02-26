#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_DIR 4
#define DEFAULT_SIZE 5

//--------------------------------------------------------------------------------------
//                                Structures

typedef struct cell_s *cell;

// this structure is a LINKED CELL, that cells compose the board game.
struct cell_s {
  piece piec;     // cell holds a piece
  direction dir;  // with a direction
  cell L;         // pointers to neighbor cells
  cell R;         // so value == NULL if this cell is a border.
  cell U;         // this struct is similar to a doubly linked list
  cell D;         // but in 2D.
  // L,R,U,D -> Respectively Left, Right, Up, Down
  direction
      dirDefault;  // this is the direction by default of the piece, initialized
                   // at the creation of the game, used to reset the board.
};

// game board structure:
struct game_s {
  bool wrapped;  // boolean to know if the game is in tore mode.
  int width;
  int height;
  cell origin;  // first cell at (0,0) (down-left corner), all other cells of
                // the board will be linked from this point.
};

typedef struct game_s *game;

typedef const struct game_s *cgame;

//--------------------------------------------------------------------------------------
//                                Static functions
//                         These functions are primitives to use cell and game
//                         correcly in net_text function
// cell primitives
static cell allocCell();
static void freeCell(cell c);
static cell getRight(cell c);
static cell getLeft(cell c);
static cell getUp(cell c);
static cell getDown(cell c);
static piece getCellPiece(cell c);
static void setCellPiece(cell c, piece p);
static direction getCellDir(cell c);
static void setCellDir(cell c, direction dir);
static direction getCellDirDef(cell c);
static void setCellDirDef(cell c, direction dir);
static bool isOutOfBound(cell c);
static cell makeOutOfBound();
static cell getCell(cell c, int x, int y);
static void setRight(cell c, cell target);
static void setLeft(cell c, cell target);
static void setUp(cell c, cell target);
static void setDown(cell c, cell target);

// game primitives
static cell getOrigin(cgame board);
static void setOrigin(game board, cell c);
static void setHeight(game board, int height);
static void setWidth(game board, int width);
static int getHeight(cgame board);
static int getWidth(cgame board);
static bool **allocBoolDoubleArray(int x, int y);
static void freeDoubleArray(void **table, int x);
static void getCoordFromDir(direction dir, int *x, int *y);
static bool isBranchOver(cgame g, cell c, direction dir, bool **checked, int x,
                         int y);

static void setWrap(game board, bool b);
static bool getWrap(cgame board);
//--------------------------------------------------------------------------------------
//                                 Prototyped functions
//                 NB: headers documentation for this functions are in the .h of
//                 this file.

game new_game_empty() {
  game g = new_game_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false);

  return g;
}

game new_game_empty_ext(int width, int height, bool wrapping) {
  game g = (game)malloc(sizeof(struct game_s));
  assert(g);
  setWidth(g, width);
  setHeight(g, height);
  setWrap(g, wrapping);

  cell originCell = allocCell();
  assert(originCell);
  setOrigin(g, originCell);

  // this part create and link cells of the board game, from (0,0) to up-right

  cell p = originCell;           // set pointer p to (0,0) cell
  cell pOld = makeOutOfBound();  // set polD to NULL cell
  for (int y = 0; y < getHeight(g); y++) {
    if (y > 0) {        // block for init cell at row > 0
      pOld = p;         // pOld become the precedent p
      p = allocCell();  // and p become a new cell
      assert(p);
    }
    setDown(p, pOld);  // we link that new cell at (0,y) with the cell under

    if (y > 0)
      setUp(pOld, p);  // and we link the down cell with curretn, only if we are
                       // not on the down row

    cell px = p;  // px pointer will sweep the current row
    cell pxOld = pOld;

    for (int x = 1; x < getWidth(g); x++) {  // setting rest of the row
      if (y > 0)
        pxOld = getRight(pxOld);  // deplace pxOld. (only if its not at row -1),
                                  // its why the parent for begin to 1.

      cell newC = allocCell();  // create cell
      assert(newC);
      setRight(px, newC);  // horizontal link
      setLeft(newC, px);

      setDown(newC, pxOld);  // vertical link
      if (y > 0) setUp(pxOld, newC);

      px = newC;  // current pointer became the created cell, that line is
                  // equivalent to move px to Right
    }
  }

  // but if wrapping == true, this part link border of the board.
  if (wrapping) {
    cell c = originCell;
    cell c2 = getCell(c, 0, getHeight(g) - 1);
    for (int i = 0; i < getWidth(g); i++) {  // link of upper row with lower row
      setUp(c2, c);
      setDown(c, c2);
      c = getRight(c);
      c2 = getRight(c2);
    }

    c = originCell;
    c2 = getCell(c, getWidth(g) - 1, 0);
    for (int i = 0; i < getHeight(g);
         i++) {  // link right column with left column
      setRight(c2, c);
      setLeft(c, c2);
      c = getUp(c);
      c2 = getUp(c2);
    }
  }

  return g;
}

game new_game(piece *pieces, direction *initial_directions) {
  game g = new_game_ext(DEFAULT_SIZE, DEFAULT_SIZE, pieces, initial_directions,
                        false);
  return g;
}

game new_game_ext(int width, int height, piece *pieces,
                  direction *initial_directions, bool wrapping) {
  // create a empty game
  game g = new_game_empty_ext(width, height, wrapping);

  // scan the board to set all dir and type of piece in each cell:

  cell pY = getOrigin(g);

  for (int y = 0; y < getHeight(g); y++) {
    cell pX = pY;
    for (int x = 0; x < getWidth(g); x++) {
      setCellPiece(pX, pieces[x + getWidth(g) * y]);
      setCellDir(pX, initial_directions[x + getWidth(g) * y]);
      setCellDirDef(pX, initial_directions[x + getWidth(g) * y]);
      pX = getRight(pX);
    }
    pY = getUp(pY);
  }

  return g;
}

void set_piece(game board, int x, int y, piece new_piece, direction dir) {
  assert(board);
  cell c = getCell(getOrigin(board), x, y);
  setCellDir(c, dir);
  setCellPiece(c, new_piece);
}

bool is_wrapping(cgame board) {
  assert(board);
  return getWrap(board);
}

void shuffle_dir(game board) {
  assert(board);

  cell pY = getOrigin(board);

  // Board swipe loops :
  for (int y = 0; y < getHeight(board); y++) {
    cell pX = pY;
    for (int x = 0; x < getWidth(board); x++) {
      direction value = rand() % NB_DIR;  // Take a random direction
      setCellDir(pX, value);
      pX = getRight(pX);
    }
    pY = getUp(pY);
  }
}

int game_height(cgame board) {
  assert(board);
  return getHeight(board);
}

int game_width(cgame board) {
  assert(board);
  return getWidth(board);
}

void rotate_piece_one(game board, int x, int y) {
  assert(board);
  if (x < 0 || x >= game_width(board) || y < 0 || y >= game_height(board)) {
    fprintf(stderr, "Error, cell is out of bounds!\n");
    return;
  }
  rotate_piece(board, x, y, 1);  // Rotate a piece once is the same as using
                                 // rotate_piece with 1 quarter turn
}

void rotate_piece(game board, int x, int y, int nb_cw_quarter_turn) {
  assert(board);
  if (x < 0 || x >= game_width(board) || y < 0 || y >= game_height(board)) {
    fprintf(stderr, "Error, cell is out of bounds!\n");
    return;
  }

  if (nb_cw_quarter_turn < 0) {  // Converts counterclockwise rotations to
                                 // clockwise (going from negative to positive)
    nb_cw_quarter_turn = NB_DIR - nb_cw_quarter_turn;
  }

  nb_cw_quarter_turn =
      nb_cw_quarter_turn %
      NB_DIR;  // Removes useless iterations since rotating a piece NB_DIR times
               // is equivalent to do nothing

  cell c = getCell(getOrigin(board), x, y);
  direction dir = getCellDir(c);
  while (nb_cw_quarter_turn > 0) {
    dir++;
    nb_cw_quarter_turn--;
  }
  setCellDir(c, dir % NB_DIR);
}

void set_piece_current_dir(game board, int x, int y, direction dir) {
  assert(board);
  cell c = getCell(getOrigin(board), x, y);
  setCellDir(c, dir);
}

bool is_edge_coordinates(cgame board, int x, int y, direction dir) {
  assert(board);
  return is_edge(get_piece(board, x, y), get_current_dir(board, x, y),
                 dir);  // test if the (x,y)piece on the board has a connection
                        // in the given direction.
}

bool is_edge(piece test_piece, direction orientation, direction dir) {
  assert(!(orientation < 0 || orientation > 3 || dir < 0 || dir > 3));
  // this code use tips with enum integer values
  switch (test_piece) {
    case EMPTY:
      return false;  // empty is always connected to nothing
      break;
    case LEAF:
      return (orientation == dir);  // a leaf is connected to its direction
      break;
    case SEGMENT:
      return (
          orientation % 2 ==
          dir % 2);  // a segment is connected to its direction and at its back
      break;
    case CORNER:
      return (orientation == dir ||
              (orientation + 1) % NB_DIR ==
                  dir);  // corner is connected in his direction and with the
                         // direction at 1/4 clockwise
      break;
    case TEE:
      return ((orientation - 1) % NB_DIR == dir || orientation == dir ||
              (orientation + 1) % NB_DIR ==
                  dir);  // tee, same as corner but with an additional
                         // connection at 1/4 counterclockwise direction
      break;
    case CROSS:
      return true;  // cross is always connected
      break;
    default:
      fprintf(stderr,
              "Error in function is_egde(piece piece, direction orientation, "
              "direction dir): first argument is not a piece!\n");
      exit(EXIT_FAILURE);
      break;
  }
}

direction opposite_dir(direction dir) {
  if (dir == N) return S;
  if (dir == E) return W;
  if (dir == S) return N;
  if (dir == W) return E;
  fprintf(stderr,
          "Error when trying to get the opposite direction, argument is not a "
          "direction!\n");
  exit(EXIT_FAILURE);
}

game copy_game(cgame game_src) {
  assert(game_src);
  // generate new game object
  game copy = new_game_empty_ext(getWidth(game_src), getHeight(game_src),
                                 getWrap(game_src));

  cell pY = getOrigin(game_src);
  cell pYC = getOrigin(copy);
  // and copy all cells
  for (int y = 0; y < getHeight(game_src); y++) {
    cell pX = pY;
    cell pXC = pYC;
    for (int x = 0; x < getWidth(game_src); x++) {
      setCellDir(pXC, getCellDir(pX));
      setCellPiece(pXC, getCellPiece(pX));

      pX = getRight(pX);
      pXC = getRight(pXC);
    }
    pY = getUp(pY);
    pYC = getUp(pYC);
  }

  return copy;
}

void delete_game(game board) {
  cell pY =
      getCell(getOrigin(board), game_width(board) - 1, game_height(board) - 1);

  // Board swipe loops, begin at top-right:
  for (int y = game_height(board) - 1; y >= 0; y--) {
    cell pX = pY;
    pY = getDown(pY);
    for (int x = game_width(board) - 1; x >= 0; x--) {
      cell c = pX;
      pX = getLeft(pX);
      freeCell(c);
    }
  }

  free(board);
  board = NULL;
}

piece get_piece(cgame board, int x, int y) {
  assert(board);
  return getCellPiece(getCell(getOrigin(board), x, y));
}

direction get_current_dir(cgame board, int x, int y) {
  assert(board);
  return getCellDir(getCell(getOrigin(board), x, y));
}

bool is_game_over(cgame g) {
  assert(g);
  int width = getWidth(g);
  int height = getHeight(g);
  bool **checked = allocBoolDoubleArray(
      width, height);  // this array represent the grid of the game and is used
                       // to keep track of which element has been checked or not
  checked[0][0] = true;  // the actual cell (the first one is the origin (0,0))
                         // is considered checked before checking the others
  int x2, y2;
  for (direction d = N; d < NB_DIR; d++) {  // for each direction
    if (is_edge(getCellPiece(getOrigin(g)), getCellDir(getOrigin(g)),
                d)) {  // if the cell is connected to this direction we check if
                       // the branch is well formed
      getCoordFromDir(d, &x2, &y2);
      if (!isBranchOver(g, getCell(getOrigin(g), x2, y2), opposite_dir(d),
                        checked, (x2 + getWidth(g)) % getWidth(g),
                        (y2 + getHeight(g)) % getHeight(g))) {
        // one of the branch starting from the origin isn't well formed
        freeDoubleArray((void **)checked, width);
        return false;
      }
    }
  }
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (!checked[i][j]) {
        // not every cell has been checked, the game is not completely connected
        freeDoubleArray((void **)checked, width);
        return false;
      }
    }
  }
  freeDoubleArray((void **)checked, width);
  return true;
}

void restart_game(game g) {
  assert(g);

  cell pY = getOrigin(g);
  // for all pieces in the board, restore the default dir value to the piece
  for (int y = 0; y < getHeight(g); y++) {
    cell pX = pY;
    for (int x = 0; x < getWidth(g); x++) {
      setCellDir(pX, getCellDirDef(pX));

      pX = getRight(pX);
    }
    pY = getUp(pY);
  }
}

//--------------------------------------------------------------------------------------
//                                Static functions bodies

/**
 * @brief Allocate dynamically a cell, a case of the board
 *
 * @return the pointer to the created cell or NULL in case of error
 **/
cell allocCell() {
  cell c = (cell)malloc(sizeof(struct cell_s));
  if (!c) {
    fprintf(stderr, "Not enough memory to alloc a cell!\n");
    return NULL;
  }
  // set default values
  c->piec = EMPTY;
  c->dir = N;
  c->L = NULL;
  c->R = NULL;
  c->U = NULL;
  c->D = NULL;
  c->dirDefault = N;
  return c;
}

/**
 * @brief Free a cell
 *
 * @param cell pointer to free
 **/
static void freeCell(cell c) {
  if (c != NULL) {
    free(c);
    c = NULL;
  }
}

/**
 * @brief Get pointer to the right of this one
 *
 * @param c, the current cell
 * @return the cell at right of the current cell or NULL in case of error
 **/
static cell getRight(cell c) {
  if (c != NULL) {
    return c->R;
  }
  fprintf(stderr, "Warning, getRight was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get pointer to the case to the left of this one
 *
 * @param c, the current cell
 * @return the cell at left of the current cell or NULL in case of error
 **/
static cell getLeft(cell c) {
  if (c != NULL) {
    return c->L;
  }
  fprintf(stderr, "Warning, getLeft was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get pointer to the above this one
 *
 * @param c, the current cell
 * @return the cell at up of the current cell or NULL in case of error
 **/
static cell getUp(cell c) {
  if (c != NULL) {
    return c->U;
  }
  fprintf(stderr, "Warning, getUp was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get pointer to the case below this one
 *
 * @param c, the current cell
 * @return the cell at down of the current cell or NULL in case of error
 **/
static cell getDown(cell c) {
  if (c != NULL) {
    return c->D;
  }
  fprintf(stderr, "Warning, getDown was used on a NULL pointer\n");
  return NULL;
}

/**
 * @brief Get the piece value of the cell
 *
 * @param c, the current cell
 * @return the piece value or EMPTY in case of error
 **/
static piece getCellPiece(cell c) {
  if (c != NULL) {
    return c->piec;
  }
  fprintf(stderr, "Warning, getCellPiece was used on a NULL pointer\n");
  return EMPTY;
}

/**
 * @brief Set a cell's piece field
 *
 * @param c, the cell we want to set the piece
 * @param p, the piece we want to set
 **/
static void setCellPiece(cell c, piece p) {
  if (c != NULL)
    c->piec = p;
  else
    fprintf(stderr, "Warning, tried to set a piece on a NULL Cell");
}

/**
 * @brief Get the direction value of the cell
 *
 * @param c, the current cell
 * @return the direction value or N in case of error
 **/
static direction getCellDir(cell c) {
  if (c != NULL) {
    return c->dir;
  }
  fprintf(stderr, "Warning, getCellDir was used on a NULL pointer\n");
  return N;
}

/**
 * @brief Set a cell's dir field
 *
 * @param c, the cell we want to set the dir
 * @param dir, the direction we want to set
 **/
static void setCellDir(cell c, direction dir) {
  if (c != NULL)
    c->dir = dir;
  else
    fprintf(stderr, "Warning, tried to set a direction on a NULL Cell");
}

/**
 * @brief Get the default direction value of the cell
 *
 * @param c, the current cell
 * @return the default direction value or N in case of error
 **/
static direction getCellDirDef(cell c) {
  if (c != NULL) {
    return c->dirDefault;
  }
  fprintf(stderr, "Warning, getDir was used on a NULL pointer\n");
  return N;
}

/**
 * @brief Set the default direction value of the cell
 *
 * @param c, the current cell
 * @return the default direction value
 **/
static void setCellDirDef(cell c, direction dir) {
  if (c != NULL) {
    c->dirDefault = dir;
  } else {
    fprintf(stderr, "Warning, can't set a default direction to a NULL cell\n");
  }
}

/**
 * @brief Check if a cell is out of board's rims.
 *
 * @param c, the cell to test
 * @return true if the cell is not in the game bound.
 **/
static bool isOutOfBound(cell c) { return c == NULL; }

/**
 * @brief Create a undefined cell
 *
 * @return undefined cell
 **/
static cell makeOutOfBound() { return NULL; }

/**
 * @brief Get the pointer to the cell that is x to the right and y above
 *
 * @param c, the current cell (origin cell in most of case)
 * @param x, how much on the left (x<0) or right (x>0) you want to move
 * @param y, how much down (y<0) or up (y>0) you want to move
 * @return pointer to a cell or NULL in case of error
 **/
static cell getCell(cell c, int x, int y) {
  // NB: a frequent use is : getCell(origin, x , y); which get cell at (x,y) in
  // the board

  if (!c) {
    fprintf(stderr, "Warning, trying to getCell from a NULL pointer!\n");
    return NULL;
  }

  while (x != 0) {
    // Tests if the cell we're trying to access is out of bound
    if (!c) {
      fprintf(stderr, "Error when trying to get cell, x was out of range!\n");
      return NULL;
    }

    if (x < 0) {
      c = c->L;
      x++;
    }

    else {
      c = c->R;
      x--;
    }
  }

  while (y != 0) {
    if (!c && y != 0) {
      fprintf(stderr, "Error when trying to get cell, y was out of range!\n");
      return NULL;
    }

    if (y < 0) {
      c = c->D;
      y++;
    }

    else {
      c = c->U;
      y--;
    }
  }

  return c;
}

/**
 * @brief Link the cell c to the target cell with the target being on the right
 *
 * @param c, the cell which is going to be linked
 * @param target, the cell to which we link c to
 **/
static void setRight(cell c, cell target) {
  if (c != NULL && target != NULL) {
    // dual link
    c->R = target;
    target->L = c;
  } else {
    if (c != NULL) {
      // border link
      c->R = NULL;
    } else {
      fprintf(stderr, "Warning, can't link a NULL pointer to the target!\n");
    }
  }
}

/**
 * @brief Link the cell c to the target cell with the target being on the left
 *
 * @param c, the cell which is going to be linked
 * @param target, the cell to which we link c to
 **/
static void setLeft(cell c, cell target) {
  if (c != NULL && target != NULL) {
    c->L = target;
    target->R = c;
  } else {
    if (c != NULL) {
      c->L = NULL;
    } else {
      fprintf(stderr, "Warning, can't link a NULL pointer to the target!\n");
    }
  }
}

/**
 * @brief Link the cell c to the target cell with the target being above
 *
 * @param c, the cell which is going to be linked
 * @param target, the cell to which we link c to
 **/
static void setUp(cell c, cell target) {
  if (c != NULL && target != NULL) {
    c->U = target;
    target->D = c;
  } else {
    if (c != NULL) {
      c->U = NULL;
    } else {
      fprintf(stderr, "Warning, can't link a NULL pointer to the target!\n");
    }
  }
}

/**
 * @brief Link the cell c to the target cell with the target being below
 *
 * @param c, the cell which is going to be linked
 * @param target, the cell to which we link c to
 **/
static void setDown(cell c, cell target) {
  if (c != NULL && target != NULL) {
    c->D = target;
    target->U = c;
  } else {
    if (c != NULL) {
      c->D = NULL;
    } else {
      fprintf(stderr, "Warning, can't link a NULL pointer to the target!\n");
    }
  }
}

/**
 * @brief Get the origin field of a board
 *
 * @param board, the board we want to get the (0,0) cell from
 * @return a pointer to a cell or NULL in case of error
 **/
static cell getOrigin(cgame board) {
  if (board != NULL) return board->origin;
  fprintf(stderr, "Warning, tried to get origin from a NULL board");
  return NULL;
}

/**
 * @brief Set the origin field of a board
 *
 * @param board, the board we want to set the origin on
 * @param c, the cell we want who will be the origin of the board
 **/
static void setOrigin(game board, cell c) {
  if (board != NULL)
    board->origin = c;
  else
    fprintf(stderr, "Warning, tried to set origin on a NULL board");
}

/**
 * @brief Set the height of a board
 *
 * @param board, the board we want to set the height on
 * @param height, the height we want the board to be
 **/
static void setHeight(game board, int height) {
  if (board != NULL)
    board->height = height;
  else
    fprintf(stderr, "Warning, tried to set height on a NULL board");
}

/**
 * @brief Set the width of a board
 *
 * @param board, the board we want to set the width on
 * @param width, the width we want the board to be
 **/
static void setWidth(game board, int width) {
  if (board != NULL)
    board->width = width;
  else
    fprintf(stderr, "Warning, tried to set width on a NULL board");
}

/**
 * @brief Get the height of a board
 *
 * @param board, the board we want to get the height
 * @return board's height or -1 in case of error
 **/
static int getHeight(cgame board) {
  if (board != NULL) return board->height;
  fprintf(stderr, "Warning, tried to get height from a NULL board");
  return -1;
}

/**
 * @brief Get the width of a board
 *
 * @param board, the board we want to get the width
 * @return board's width or -1 in case of error
 **/
static int getWidth(cgame board) {
  if (board != NULL) return board->width;
  fprintf(stderr, "Warning, tried to get width from a NULL board");
  return -1;
}

/**
 * @brief Set the wrapped parameter of a board
 *
 * @param board, the board we want to set the arg
 * @param bool, the cell we want who will be the origin of the board
 **/
static void setWrap(game board, bool b) {
  if (board != NULL)
    board->wrapped = b;
  else
    fprintf(stderr, "Warning, tried to set wrap on a NULL board");
}
/**
 * @brief Get the wrap parameter of a board
 *
 * @param board, the board we want to get the wrap
 * @return wrap boolean or -1 in case of error
 **/
static bool getWrap(cgame board) {
  if (board != NULL) return board->wrapped;
  fprintf(stderr, "Warning, tried to get wrap from a NULL board");
  return -1;
}

//--------------- utilities functions for is_game_over() --------------

/**
 * @brief Allocate the necessary memory for a double array of booleans, with a
 * size of x*y, initialized to false.
 *
 * @param x, the number of columns for the double array
 * @param y, the number of lines for the double array
 * @return the adress of the double array
 */
static bool **allocBoolDoubleArray(int x, int y) {
  bool **table = (bool **)malloc(sizeof(bool *) * (size_t)x);
  if (!table) {
    fprintf(stderr, "not enough memory to alloc a table of %d pointers", x);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < x; i++) {
    table[i] = (bool *)malloc(sizeof(bool) * (size_t)y);
    if (!table[i]) {
      fprintf(stderr, "not enough memory to alloc a table of %d booleans", y);
      exit(EXIT_FAILURE);
    }
    for (int j = 0; j < y; j++) {
      table[i][j] = false;
    }
  }
  return table;
}

/**
 * @brief Free the memory memory used by a double array of x*y.
 *
 * @param table, the double array to free
 * @param x, the number of columns for the double array
 */
void freeDoubleArray(void **table, int x) {
  if (table) {
    for (int i = 0; i < x; i++) {
      if (table[i]) {
        free(table[i]);
      }
    }
    free(table);
    table = NULL;
  }
}

/**
 * @brief Transform a direction into coordinates
 *
 * @param dir, the direction we want to convert
 * @param x, a pointer to the x value of the coordinates
 * @param y, a pointer to the y value of the coordinates
 */
static void getCoordFromDir(direction dir, int *x, int *y) {
  if (!x || !y) {
    fprintf(
        stderr,
        "Warning, transmitted a NULL pointer, getCoordFromDir couldn't work!");
    return;
  }
  *x = 0;
  *y = 0;
  switch (dir) {
    case N:
      *y = 1;
      break;
    case S:
      *y = -1;
      break;
    case E:
      *x = 1;
      break;
    case W:
      *x = -1;
      break;
    default:
      fprintf(stderr, "Warning, unknown direction given");
      break;
  }
}

/**
 * @brief Recursive function to check if a branch (a series of connected cells)
 *is well connected, ends by a LEAF and contain no loop (this function is used
 *in is_game_over)
 *
 * @param g, the game in which the branch is checked
 * @param c, the cell from where the branch start
 * @param dir, the direction from where the branch come from
 * @param checked, a double array of boolean to verify if the cell has already
 *been checked or not
 * @param x, the x coordinate of the cell
 * @param y, the y coordinate of the cell
 * @return true if the branch is well formed, false otherwise
 **/
static bool isBranchOver(cgame g, cell c, direction dir, bool **checked, int x,
                         int y) {
  if (isOutOfBound(c) || !is_edge(getCellPiece(c), getCellDir(c), dir)) {
    // the branch is disconnected or doesn't end by a leaf
    return false;
  }

  direction currentDir = getCellDir(c);
  piece currentPiece = getCellPiece(c);

  if (checked[x][y]) {
    // the program has already checked this cell, there is a loop!
    return false;
  }
  // this cell is well connected to the previous one : it has been checked
  checked[x][y] = true;

  // Now we need to verify that it is well connected to further cells of the
  // branch
  int x2, y2;
  for (direction d = 0; d < NB_DIR; d++) {
    if (d != dir &&
        is_edge(currentPiece, currentDir,
                d)) {  // for each direction where the cell is connected (except
                       // the one the branch is coming from) we check if the
                       // branch is well formed further
      getCoordFromDir(d, &x2, &y2);
      if (!isBranchOver(g, getCell(c, x2, y2), opposite_dir(d), checked,
                        (x + x2 + getWidth(g)) % getWidth(g),
                        (y + y2 + getHeight(g)) % getHeight(g))) {
        // the branch is not well formed further!
        return false;
      }
    }
  }
  // everything from this cell point is well formed
  return true;
}
