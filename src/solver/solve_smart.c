#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool_array.h"
#include "game.h"
#include "game_io.h"

#define NB_DIR 4
#define NB_DIR_SEGMENT 2

// Global variables to avoid copying them in each function call
direction DIRS[] = {N, E, S, W};
game g = NULL;
bool **checked = NULL;
bool **unmovable = NULL;

//--------------------------------------------------------------------------------------
//                                Structures
typedef struct possibility_s *possibility;

// this structure is used as a chained list to save different dispositions of
// pieces.
struct possibility_s {
  uint16_t x;     // the coordinates of the corresponding cell in the game
  uint16_t y;     //
  direction dir;  // the direction to put this piece in in order to set the game
                  // with the saved possibility
  bool isLeaf;    // a boolean to indicate if this possibility is a leaf or not
  uint32_t nbNextPos;  // the number of deriving possibilities from this cell (0
                       // if it is a leaf)
  uint32_t
      *nbNextDerivPos;  // an array of the number of leaves we can access from
                        // the deriving possibilities of this possibility
  uint32_t totalNextDerivPos;  // the number of leaves we can access from this
                               // possibility
  possibility
      *nextPos;  // an array of the deriving possibilities from this cell
};

//--------------------------------------------------------------------------------------
//                                Game solving functions
//                         These functions are solving the game accordingly to
//                         the first argument given (FIND_ONE/NB_SOL/FIND_ALL)
bool find_one(char *argv[]);
bool nb_sol(char *argv[]);
bool find_all(char *argv[]);

bool find_one_sdl(game g);

//--------------------------------------------------------------------------------------
//                                Error handler functions
//                         These functions are used by find_one/nb_sol/find_all
//                         to handle errors and properly leave the function, to
//                         tests if the arguments are valid, as well as to show
//                         how to use the solver
static bool gameLoadError();
static bool solFileError(game board);

//--------------------------------------------------------------------------------------
//                                Static functions
//                         These functions are primitives used in the differents
//                         solvers
static possibility findSolution(uint16_t x, uint16_t y);
static possibility allocPossibility();
static void freePossibility(possibility pos);
static void freeChainPossibility(possibility pos);
static possibility createSinglePoss(uint16_t x, uint16_t y, direction dir);
static void addBranchPoss(possibility poss, possibility chainPoss);
// static possibility getLeaf(possibility poss, uint32_t numLeaf); Commented
// because not used
static void spreadLeaf(possibility poss, uint32_t numLeaf, uint32_t nbPossToAdd,
                       possibility *possToAdd, uint32_t nbDerivPos);
static possibility delLeaf(possibility poss, uint32_t numLeaf);
static void getCoordFromDir(direction dir, int32_t *x, int32_t *y);
static bool setUnmovable();
static bool setRecUnmovable(uint16_t x, uint16_t y);
static possibility propagate(uint16_t x, uint16_t y);
static void loadPossibility(possibility poss, uint32_t numPoss);
static void unloadPossibility(possibility poss, uint32_t numPoss);
static uint32_t findPoss(possibility *possArray, uint32_t *nbDerivPos,
                         uint16_t x, uint16_t y);
static bool isGoodDir(uint16_t x, uint16_t y);

//--------------------------------------------------------------------------------------
//                                Game solving functions bodies

/**
 * @brief Finds a single solution and writes it in a .sol file
 *
 * @param argv, an array of the arguments
 * @return false in case of error, true otherwise
 **/
bool find_one(char *argv[]) {
  g = load_game(argv[2]);
  if (!g) return gameLoadError();

  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  checked = alloc_double_bool_array(width, height);
  unmovable = alloc_double_bool_array(width, height);
  possibility solution = findSolution(0, 0);

  if (solution != NULL) {
    loadPossibility(solution, 0);
    save_game(g, strcat(argv[3], ".sol"));
    freeChainPossibility(solution);
  } else {
    FILE *f = fopen(strcat(argv[3], ".sol"), "w");
    FPRINTF(f, "NO SOLUTION\n");
    fclose(f);
  }

  free_double_bool_array(checked, width);
  checked = NULL;
  free_double_bool_array(unmovable, width);
  unmovable = NULL;
  delete_game(g);
  g = NULL;
  return true;
}

/**
 * @brief Finds how many solution there are and write it in a .nbsol file
 *
 * @param argv, an array of the arguments
 * @return false in case of error, true otherwise
 **/
bool nb_sol(char *argv[]) {
  g = load_game(argv[2]);
  if (!g) return gameLoadError();

  FILE *fSolution = fopen(strcat(argv[3], ".nbsol"), "w");
  if (!fSolution) return solFileError(g);
  uint32_t result = 0;

  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  checked = alloc_double_bool_array(width, height);
  unmovable = alloc_double_bool_array(width, height);
  possibility solution = findSolution(0, 0);

  if (solution != NULL) {
    result = solution->totalNextDerivPos;
    freeChainPossibility(solution);
  }
  FPRINTF(fSolution, "%u\n", result);

  fclose(fSolution);

  free_double_bool_array(checked, width);
  checked = NULL;
  free_double_bool_array(unmovable, width);
  unmovable = NULL;
  delete_game(g);
  g = NULL;
  return true;
}

/**
 * @brief Finds all the solutions and writes it in .solN files, with N ∈ [1,
 *NB_SOL]
 *
 * @param argv, an array of the arguments
 * @return false in case of error, true otherwise
 **/
bool find_all(char *argv[]) {
  g = load_game(argv[2]);
  if (!g) return gameLoadError();

  // Multiple solution files must be created here
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  checked = alloc_double_bool_array(width, height);
  unmovable = alloc_double_bool_array(width, height);

  possibility solution = findSolution(0, 0);

  if (solution != NULL) {
    for (uint32_t i = 0; i < solution->totalNextDerivPos; i++) {
      loadPossibility(solution, i);
      char sol_num[10];
      sprintf(sol_num, "%u", i + 1);
      char fNameCopy[100];
      save_game(g, strcat(strcat(strcpy(fNameCopy, argv[3]), ".sol"), sol_num));
    }
    freeChainPossibility(solution);
  }

  free_double_bool_array(checked, width);
  checked = NULL;
  free_double_bool_array(unmovable, width);
  unmovable = NULL;
  delete_game(g);
  g = NULL;
  return true;
}

/**
 * @brief Finds one solution and applies it to the given game (used for SDL)
 *
 * @param board, the game to solve
 * @return false in case of error, true otherwise
 **/
bool find_one_sdl(game board) {
  g = board;
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  checked = alloc_double_bool_array(width, height);
  unmovable = alloc_double_bool_array(width, height);
  possibility solution = findSolution(0, 0);

  bool status = true;
  if (solution != NULL) {
    loadPossibility(solution, 0);
    freeChainPossibility(solution);
  } else
    status = false;

  free_double_bool_array(checked, width);
  checked = NULL;
  free_double_bool_array(unmovable, height);
  unmovable = NULL;
  g = NULL;
  return status;
}
//--------------------------------------------------------------------------------------
//                                Error handler functions bodies

/**
 * @brief Prints the error in stderr if a game couldn't be loaded
 * @return false
 **/
static bool gameLoadError() {
  FPRINTF(stderr, "Error when loading the game from file!\n");
  return false;
}

/**
 * @brief Prints the error in stderr if a solution file couldn't be created and
 *delete the loaded game
 *
 * @param g, the game loaded before the file was created
 * @return false
 **/
static bool solFileError(game board) {
  if (board != NULL) delete_game(board);
  FPRINTF(stderr, "Error when attempting to create the solution file(s)!\n");
  return false;
}

// functions for debugging
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
/*
static void displayBoolArray(bool **a, int w, int h) {
  for (int i = h - 1; i >= -1; i--) {
    FPRINTF(stderr, "%d", i);
    if (i != -1) {
      FPRINTF(stderr, " ");
      for (int j = 0; j < w; j++) {
        FPRINTF(stderr, "%d", a[j][i]);
      }
    } else {
      for (int j = 0; j < w; j++) {
        FPRINTF(stderr, "%d", j);
      }
    }
    FPRINTF(stderr, "\n");
  }
  FPRINTF(stderr, "\n");
}

static void displayPoss(possibility poss) {
  FPRINTF(stderr, "x=%u\n", poss->x);
  FPRINTF(stderr, "y=%u\n", poss->y);
  FPRINTF(stderr, "dir=%d\n", poss->dir);
  FPRINTF(stderr, "isLeaf=%d\n", poss->isLeaf);
  FPRINTF(stderr, "nbNextPoss=%u\n", poss->nbNextPos);
  for (uint32_t i = 0; i < poss->nbNextPos; i++) {
    FPRINTF(stderr, "nbNextDerivPos[%u]=%u\n", i, poss->nbNextDerivPos[i]);
  }
  FPRINTF(stderr, "totalNextDerivPoss=%u\n", poss->totalNextDerivPos);
  for (uint32_t i = 0; i < poss->nbNextPos; i++) {
    if (poss->nextPos[i] == NULL) {
      FPRINTF(stderr, "nextPos[%u]=NULL\n", i);
    } else {
      FPRINTF(stderr, "nextPos[%u]=%p\n", i, poss->nextPos[i]);
    }
  }
  FPRINTF(stderr, "\n");
}

static void displayPossTree(possibility poss, uint32_t indice) {
  if (poss == NULL) {
    FPRINTF(stderr, "POSS is empty (NULL) !\n");
    return;
  }
  FPRINTF(stderr, "displaying Poss num%u\n", indice);
  displayPoss(poss);
  if (poss->isLeaf) {
    return;
  }
  for (uint32_t i = 0; i < poss->nbNextPos; i++) {
    FPRINTF(stderr, "calling poss from poss num%u\n", indice);
    displayPossTree(poss->nextPos[i], indice + 1);
  }
}*/
/*
  FPRINTF(stderr, "displaying checked!\n");
  displayBoolArray(checked, game_width(g), game_height(g));
  FPRINTF(stderr, "displaying unmovable!\n");
  displayBoolArray(unmovable, game_width(g), game_height(g));
  */

//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************

//--------------------------------------------------------------------------------------
//                                Static functions bodies
/**
 *
 *
 *
 */
static possibility findSolution(uint16_t x, uint16_t y) {
  possibility possFound[NB_DIR], thisPoss;
  uint32_t nbPossFound, nbDerivPos;
  thisPoss = NULL;

  uint16_t width = game_width(g);
  uint16_t height = game_height(g);

  if (setUnmovable()) {
    if (check_double_bool_array(unmovable, width, height)) {
      thisPoss = createSinglePoss(0, 0, get_current_dir(g, 0, 0));
      thisPoss->totalNextDerivPos = 1;
    } else {
      thisPoss = createSinglePoss(0, 0, 0);
      nbPossFound = findPoss(possFound, &nbDerivPos, x, y);
      spreadLeaf(thisPoss, 0, nbPossFound, possFound, nbDerivPos);
      for (uint32_t i = 0; i < thisPoss->totalNextDerivPos; i++) {
        loadPossibility(thisPoss, i);
        if (!check_double_bool_array(checked, width, height)) {
          unloadPossibility(thisPoss, i);
          thisPoss = delLeaf(thisPoss, i);
          i--;
          if (thisPoss == NULL) {
            break;
          }
        } else {
          unloadPossibility(thisPoss, i);
        }
      }
    }
  }
  return thisPoss;
}

/**
 * @brief allocate space for a cell of a possibility tree and initialise its
 *main variables
 *
 * @return the possibility (pointer to a possibility_s) created
 **/
static possibility allocPossibility() {
  possibility poss = (possibility)malloc(sizeof(struct possibility_s));
  assert(poss);
  poss->nbNextDerivPos = (uint32_t *)malloc(NB_DIR * sizeof(uint32_t));
  assert(poss->nbNextDerivPos);
  poss->nextPos = (possibility *)malloc(NB_DIR * sizeof(possibility));
  assert(poss->nextPos);
  poss->nbNextPos = 0;
  poss->x = 0;
  poss->y = 0;
  poss->dir = N;
  poss->isLeaf = true;
  poss->totalNextDerivPos = 0;
  return poss;
}

/**
 * @brief free a possibility from the memory
 *
 * @param pos, the possibility to free
 */
static void freePossibility(possibility pos) {
  if (pos != NULL) {
    if (pos->nextPos != NULL) {
      free(pos->nextPos);
    }
    if (pos->nbNextDerivPos != NULL) {
      free(pos->nbNextDerivPos);
    }
    free(pos);
  }
}

/**
 * @brief free a tree of possibility
 *
 * @param pos, the possibility at the start of the tree to free
 **/
static void freeChainPossibility(possibility pos) {
  for (uint32_t i = 0; i < pos->nbNextPos; i++) {
    freeChainPossibility(pos->nextPos[i]);
  }
  freePossibility(pos);
}

/**
 * @brief allocate and create a possibility (without any branches)
 *
 * @param x, the x coordinate of the piece
 * @param y, the y coordinate of the piece
 * @param dir, the direction to save for this piece
 * @return the possibility created
 **/
static possibility createSinglePoss(uint16_t x, uint16_t y, direction dir) {
  possibility poss = allocPossibility();
  poss->x = x;
  poss->y = y;
  poss->dir = dir;
  return poss;
}

/**
 * @brief add a possibility (or the start of a chain of possibilities) as a
 *branch of another one
 *
 * @param poss, the possibility on which the branch will be added
 * @param chainPoss, the possibility which will be added as a branch of poss
 **/
static void addBranchPoss(possibility poss, possibility chainPoss) {
  assert(poss);
  assert(chainPoss);
  if (poss->nbNextPos >= NB_DIR) {
    FPRINTF(stderr, "Not enough space to add branch of possibility\n");
    exit(EXIT_FAILURE);
  }

  poss->nextPos[poss->nbNextPos] = chainPoss;
  if (chainPoss->isLeaf) {
    poss->nbNextDerivPos[poss->nbNextPos] = 1;
    poss->totalNextDerivPos += 1;
  } else {
    poss->nbNextDerivPos[poss->nbNextPos] = chainPoss->totalNextDerivPos;
    poss->totalNextDerivPos += chainPoss->totalNextDerivPos;
  }
  poss->nbNextPos++;
  poss->isLeaf = false;
}

///////////////////// Commented because unused /////////////////////

/**
 * @brief search a leaf in a possibility tree and returns it
 *
 * @param poss, the possibility at the start of the possibility tree
 * @param numLeaf, the leaf to search for
 *
 * @return the possibility corresponding to the leaf demanded
 **/

/*
static possibility getLeaf(possibility poss, uint32_t numLeaf) {
  assert(poss);
  if (poss->isLeaf)
    return poss;  // we've reached the end of the possibility tree
  uint32_t i = 0;
  while (i < poss->nbNextPos && poss->nbNextDerivPos[i] <= numLeaf) {
    numLeaf -= poss->nbNextDerivPos[i];
    i++;
  }
  if (i >= poss->nbNextPos) {
    FPRINTF(stderr,
            "error : wrong parameter given or malformed possibility tree!\n");
    exit(EXIT_FAILURE);
  }
  return getLeaf(poss->nextPos[i], numLeaf);
}*/

////////////////////////////////////////////////////////////////////

/**
 * @brief adds branches to a leaf (which wont be a leaf anymore) and update
 *accordingly the previous possibilies (nbDerivPoss, etc.)
 *
 * @param poss, the possibility at the start of the tree from which the leaf
 *want to be extended
 * @param numLeaf, the leaf to extend
 * @param nbPossToAdd, the number of deriving possibilities (branches) to add to
 *the leaf
 * @param possToAdd, an array containing the possibilities to add
 **/
static void spreadLeaf(possibility poss, uint32_t numLeaf, uint32_t nbPossToAdd,
                       possibility *possToAdd, uint32_t nbDerivPos) {
  if (poss->isLeaf) {
    for (uint32_t j = 0; j < nbPossToAdd; j++) {
      addBranchPoss(poss, possToAdd[j]);
    }
    return;
  }

  uint32_t i = 0;
  while (i < poss->nbNextPos && poss->nbNextDerivPos[i] <= numLeaf) {
    numLeaf -= poss->nbNextDerivPos[i];
    i++;
  }
  if (i >= poss->nbNextPos) {
    FPRINTF(stderr,
            "error : wrong parameter given or malformed possibility tree!\n");
    exit(EXIT_FAILURE);
  }

  poss->nbNextDerivPos[i] += (nbDerivPos - 1);
  poss->totalNextDerivPos += (nbDerivPos - 1);
  spreadLeaf(poss->nextPos[i], numLeaf, nbPossToAdd, possToAdd, nbDerivPos);
}

/**
 * @brief delete a leaf and the previous branche from a tree and update
 *accordingly the previous possibilies (nbDerivPoss, etc.)
 *
 * @param poss, the possibility at the start of the tree from which the leaf
 *want to be removed
 * @param numLeaf, the leaf to delete
 *
 * @return poss if there is still other branches on the tree or NULL if it was
 *completely removed
 **/
static possibility delLeaf(possibility poss, uint32_t numLeaf) {
  if (poss->totalNextDerivPos <= 1) {
    freeChainPossibility(poss);
    return NULL;
  }
  uint32_t i = 0;
  while (i < poss->nbNextPos && poss->nbNextDerivPos[i] <= numLeaf) {
    numLeaf -= poss->nbNextDerivPos[i];
    i++;
  }
  if (i >= poss->nbNextPos) {
    FPRINTF(stderr,
            "error : wrong parameter given or malformed possibility tree!\n");
    exit(EXIT_FAILURE);
  }
  delLeaf(poss->nextPos[i], numLeaf);
  poss->totalNextDerivPos--;
  poss->nbNextDerivPos[i]--;
  if (poss->nbNextDerivPos[i] == 0) {
    while (i < poss->nbNextPos - 1) {
      poss->nbNextDerivPos[i] = poss->nbNextDerivPos[i + 1];
      poss->nextPos[i] = poss->nextPos[i + 1];
      i++;
    }
    poss->nbNextPos--;
  }
  if (poss->nbNextPos == 0) {
    poss->isLeaf = true;
  }
  return poss;
}

/**
 * @brief Transform a direction into coordinates
 *
 * @param dir, the direction we want to convert
 * @param x, a pointer to the x value of the coordinates
 * @param y, a pointer to the y value of the coordinates
 */
static void getCoordFromDir(direction dir, int32_t *x, int32_t *y) {
  if (!x || !y) {
    FPRINTF(
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
      FPRINTF(stderr, "Warning, unknown direction given");
      break;
  }
}

/**
 * @brief Set the unmovable double array : check if some pieces (CROSS, or TEE
 *next to a non warping border for example) can be set as unmovable before
 *lauching the solving algorithm
 *
 * @param g, the game on which the double array must be initialized
 * @param unmovable, a double array of boolean used to know which piece can be
 *moved to search for possible solutions
 *
 * @return false if a piece has no direction possible, true otherwise
 **/
static bool setUnmovable() {
  uint16_t width = game_width(g);
  uint16_t height = game_height(g);
  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      if (!unmovable[x][y]) {
        if (!setRecUnmovable(x, y)) {
          free_double_bool_array(checked, width);
          return false;
        }
      }
    }
  }
  return true;
}

/**
 *
 *
 */
static bool setRecUnmovable(uint16_t x, uint16_t y) {
  if (unmovable[x][y]) {
    return true;
  }

  uint8_t nbDir;
  if (get_piece(g, x, y) == SEGMENT) {
    nbDir = NB_DIR_SEGMENT;
  } else {
    nbDir = NB_DIR;
  }

  bool positions[NB_DIR] = {false, false, false, false};
  uint32_t nbPosition = 0;
  int32_t x2, y2;
  uint16_t x3, y3;

  if (get_piece(g, x, y) == CROSS) {
    unmovable[x][y] = true;
  }

  for (uint32_t i = 0; i < nbDir; i++) {
    set_piece_current_dir(g, x, y, DIRS[i]);
    if (isGoodDir(x, y)) {
      nbPosition++;
      positions[i] = true;
    }
  }

  if (nbPosition == 0) {
    return false;
  } else if (nbPosition == 1) {
    for (uint8_t i = 0; i < nbDir; i++) {
      if (positions[i]) {
        set_piece_current_dir(g, x, y, DIRS[i]);
        unmovable[x][y] = true;
      }
    }
  }

  if (unmovable[x][y]) {
    for (uint8_t i = 0; i < NB_DIR; i++) {
      getCoordFromDir(DIRS[i], &x2, &y2);
      x3 = (x + x2 + game_width(g)) % game_width(g);
      y3 = (y + y2 + game_height(g)) % game_height(g);
      if (!(x3 - x2 == x && y3 - y2 == y) && !is_wrapping(g)) {
        // If we're out of bounds and there are no wrapping
      } else if (!setRecUnmovable(x3, y3)) {
        return false;
      }
    }
  }
  return true;
}

/**
 * @brief a function (working recursively with findPoss) that propagate the
 *research of solution to all the connections of the current piece
 *
 * @param g, the game on which the function is working
 * @param x, the coordinate x of the cell that is currently tested in the
 *function
 * @param y, the coordinate y of the cell that is currently tested in the
 *function
 * @param checked, a double array of boolean used to verify which piece has
 *already been checked
 * @param unmovable, a double array of boolean used to know which piece can be
 *moved to search for possible solutions
 *
 * @return a possibility tree starting with the current piece in the position it
 *has just been tested or NULL if no coherent possibility has been found
 **/
static possibility propagate(uint16_t x, uint16_t y) {
  int32_t x2, y2;
  possibility thisPoss = createSinglePoss(x, y, get_current_dir(g, x, y));
  // The possibility we're going to return at the end of the function
  possibility possFound[NB_DIR];
  // Trees of possibilites will be stored in this array
  uint32_t nbPossToCheck = 1, nbPossFound, nbDerivPos;
  // By setting nbPossToCheck to 1 instead of 0 by default, we're allowed to
  // test the first direction without actually loading a proposition because
  // thissPoss is still a leaf
  for (uint8_t i = 0; i < NB_DIR; i++) {
    getCoordFromDir(DIRS[i], &x2, &y2);
    x2 = (x + x2 + game_width(g)) % game_width(g);
    y2 = (y + y2 + game_height(g)) % game_height(g);
    if (is_edge_coordinates(g, x, y, DIRS[i]) && (!checked[x2][y2])) {
      // For each direction where this piece is connected except the one it's
      // coming from
      for (uint32_t j = 0; j < nbPossToCheck; j++) {
        loadPossibility(thisPoss, j);
        // For every possibility found before on the other connections of this
        // piece
        nbPossFound =
            findPoss(possFound, &nbDerivPos, (uint16_t)x2, (uint16_t)y2);
        // We look up the possibilities with findPoss()
        unloadPossibility(thisPoss, j);
        if (nbPossFound == 0) {
          // If we cannot find a solution with the loaded possibility, it is
          // invalid thus will be deleted
          thisPoss = delLeaf(thisPoss, j);
          nbPossToCheck--;
          // Corresponds to : nbPossToCheck = thisPoss->totalNextDerivPos; (if
          // thisPoss != NULL)
          j--;
        } else {
          // Otherwise we append the solutions found at the end of the
          // possibility
          spreadLeaf(thisPoss, j, nbPossFound, possFound, nbDerivPos);
          nbPossToCheck += (nbDerivPos - 1);
          // Corresponds to : nbPossToCheck = thisPoss->totalNextDerivPos;
          j += (nbDerivPos - 1);
        }
      }
      // If after testing all the solutions there is none left, it means that
      // this piece cannot be placed this way, then the last call to delLeaf
      // deleted thisPoss (Which has been set to NULL). Since nbPossToCheck is
      // equal to 0, the last For loop won't iterate.
    }
  }
  return thisPoss;
}

/**
 * @brief load a possibility on the game: put the pieces in the corresponding
 *position and set them as checked to block them in this position in the futur
 *test
 *
 * @param poss, the start of the tree of possibility, if NULL, function will do
 *nothing
 * @param numPoss, the possibility to load (because several possibilities exists
 *in one tree), corresponds to the number of the leaf that is targetted
 * @param g, the game to modify
 * @param checked, a double array of boolean used to verify which piece has
 *already been checked
 **/
static void loadPossibility(possibility poss, uint32_t numPoss) {
  if (poss == NULL) return;  // we're trying to load an unexisting possibility
  set_piece_current_dir(g, poss->x, poss->y, poss->dir);
  checked[poss->x][poss->y] = true;
  if (poss->isLeaf) return;  // we've reached the end of the possibility to load
  uint32_t i = 0;
  while (i < poss->nbNextPos && poss->nbNextDerivPos[i] <= numPoss) {
    numPoss -= poss->nbNextDerivPos[i];
    i++;
  }
  if (i >= poss->nbNextPos) {
    FPRINTF(stderr,
            "error : wrong parameter given or malformed possibility tree!\n");
    exit(EXIT_FAILURE);
  }
  loadPossibility(poss->nextPos[i], numPoss);
}

/**
 * @brief Does the opposite of load game by setting the pieces locked by a
 *possibility loaded as unchecked
 *
 * @param poss, the start of the tree of possibility, if NULL, function will do
 *nothing
 * @param numPoss, the possibility to load (because several possibilities exists
 *in one tree), corresponds to the number of the leaf that is targetted
 * @param checked, a double array of boolean used to verify which piece has
 *already been checked
 **/
static void unloadPossibility(possibility poss, uint32_t numPoss) {
  if (poss == NULL) return;  // we're trying to unload an unexisting possibility
  checked[poss->x][poss->y] = false;
  if (poss->isLeaf)
    return;  // we've reached the end of the possibility to unload
  uint32_t i = 0;
  while (i < poss->nbNextPos && poss->nbNextDerivPos[i] <= numPoss) {
    numPoss -= poss->nbNextDerivPos[i];
    i++;
  }
  if (i >= poss->nbNextPos) {
    FPRINTF(stderr,
            "error : wrong parameter given or malformed possibility tree!\n");
    exit(EXIT_FAILURE);
  }
  unloadPossibility(poss->nextPos[i], numPoss);
}

/**
 * @brief A recursive function which search for possibilities of solution
 *(working recursively with propagate)
 *
 * @param possArray, an array containing the different possibilities the
 *function has found (do not access it if the function return 0)
 * @param g, the game on which the function has to found possibilities
 * @param x, the coordinate x of the cell that is currently tested in the
 *function
 * @param y, the coordinate y of the cell that is currently tested in the
 *function
 * @param checked, a double array of boolean used to verify which piece has
 *already been checked
 * @param unmovable, a double array of boolean used to know which piece can be
 *moved to search for possible solutions
 *
 * @return the number of possibility tree the function has found (different than
 *the total number of possibilities)
 **/
static uint32_t findPoss(possibility *possArray, uint32_t *nbDerivPos,
                         uint16_t x, uint16_t y) {
  uint8_t nbDir;
  *nbDerivPos = 0;
  if (get_piece(g, x, y) == SEGMENT) {
    nbDir = NB_DIR_SEGMENT;
  } else {
    nbDir = NB_DIR;
  }
  uint32_t nbPoss = 0;
  if (unmovable[x][y]) {
    // If this piece cannot be in another direction
    possArray[0] = propagate(x, y);
    if (possArray[0] == NULL) {
      return 0;
    } else {
      if (possArray[nbPoss]->isLeaf) {
        *nbDerivPos += 1;
      } else {
        *nbDerivPos += possArray[nbPoss]->totalNextDerivPos;
      }
      return 1;
    }
  } else {
    for (uint8_t i = 0; i < nbDir; i++) {
      // For each direction this piece can be in
      set_piece_current_dir(g, x, y, DIRS[i]);
      if (isGoodDir(x, y)) {
        // If it's suitable with the rest of the game.
        possArray[nbPoss] = propagate(x, y);
        // We propagate the solution search
        if (possArray[nbPoss] != NULL) {
          // If we found a possibility of a solution, we add it to the array of
          // possibilities found and return it, we just need to increment the
          // corresponding uint32_t size
          if (possArray[nbPoss]->isLeaf) {
            *nbDerivPos += 1;
          } else {
            *nbDerivPos += possArray[nbPoss]->totalNextDerivPos;
          }
          nbPoss++;
        }
      }
    }
  }
  return nbPoss;
}

/**
 * @brief test if a piece in this direction would fit well in the game (would
 *not cause impossible connection with neighbouring pieces and no loop either)
 *
 * @param g, the game on which the function has to verify the positions
 * @param x, the x coordinate of the piece
 * @param y, the y coordinate of the piece
 * @param checked, a double array of boolean used to know which piece as already
 *been placed by the algorithm (used to check on loops)
 * @param unmovable, a double array of boolean used to know which piece can
 *still be moved to find a solution.
 *
 * @return true if the piece can be placed in this position without problem,
 *false otherwise.
 **/
bool isGoodDir(uint16_t x, uint16_t y) {
  int32_t x2, y2;
  uint16_t x3, y3;
  bool foundChecked = false;
  for (uint16_t i = 0; i < NB_DIR; i++) {
    getCoordFromDir(DIRS[i], &x2, &y2);
    x3 = (x + x2 + game_width(g)) % game_width(g);
    y3 = (y + y2 + game_height(g)) % game_height(g);
    if (is_edge_coordinates(g, x, y, DIRS[i])) {
      if (!(x3 - x2 == x && y3 - y2 == y) && !is_wrapping(g)) {
        // If we are out of bounds and wrapping is disabled, this piece cannot
        // be in this position
        return false;
      }

      if ((checked[x3][y3] || unmovable[x3][y3]) &&
          !is_edge_coordinates(g, x3, y3, opposite_dir(DIRS[i]))) {
        // The piece to which it is connected cannot move and is not connected
        return false;
      }

      if (checked[x3][y3] &&
          is_edge_coordinates(g, x3, y3, opposite_dir(DIRS[i]))) {
        if (foundChecked) {
          // We already found a pieced that is connected and checked : place the
          // piece this way would create a loop
          return false;
        }
        foundChecked = true;
      }
    } else if ((checked[x3][y3] || unmovable[x3][y3]) &&
               is_edge_coordinates(g, x3, y3, opposite_dir(DIRS[i]))) {
      // The piece to which it is not connected cannot move and has to be
      // connected in return
      return false;
    }
  }
  return true;
}
