#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_io.h"

//--------------------------------------------------------------------------------------
//                                Game solving functions
//                         These functions are solving the game accordingly to
//                         the first argument given (FIND_ONE/NB_SOL/FIND_ALL)
bool find_one(char *argv[]);
bool nb_sol(char *argv[]);
bool find_all(char *argv[]);

//--------------------------------------------------------------------------------------
//                                Error handler functions
//                         These functions are used by find_one/nb_sol/find_all
//                         to handle errors and properly leave the function, to
//                         tests if the arguments are valid, as well as to show
//                         how to use the solver
bool gameLoadError();
bool solFileError(game g);

bool bruteError();
int brute(int type, game g, char *nameFile);

//--------------------------------------------------------------------------------------
//                                Game solving functions bodies

/**
 * @brief Finds a single solution and writes it in a .sol file
 *
 * @param argv, an array of the arguments
 * @return false in case of error, true otherwise
 **/
bool find_one(char *argv[]) {
  game g = load_game(argv[2]);
  if (!g) return gameLoadError();

  // brute mode:
  int result = brute(0, g, argv[3]);
  if (result == -1) return bruteError();
  if (result == 0) {
    FILE *f = fopen(strcat(argv[3], ".sol"), "w");
    fprintf(f, "NO SOLUTION\n");
    fclose(f);
  }
  delete_game(g);
  return true;
}

/**
 * @brief Finds how many solution there are and write it in a .nbsol file
 *
 * @param argv, an array of the arguments
 * @return false in case of error, true otherwise
 **/
bool nb_sol(char *argv[]) {
  game g = load_game(argv[2]);
  if (!g) return gameLoadError();

  FILE *fSolution = fopen(strcat(argv[3], ".nbsol"), "w");
  if (!fSolution) return solFileError(g);

  // brute mode:
  int result = brute(1, g, argv[3]);
  if (result == -1) return bruteError();

  fprintf(fSolution, "%d\n", result);

  fclose(fSolution);
  delete_game(g);
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
  game g = load_game(argv[2]);
  if (!g) return gameLoadError();

  // Multiple solution files must be created here

  // brute mode:
  int result = brute(2, g, argv[3]);
  if (result == -1) return bruteError();

  delete_game(g);
  return true;
}

//--------------------------------------------------------------------------------------
//                                Error handler functions bodies

/**
 * @brief Prints the error in stderr if a game couldn't be loaded
 * @return false
 **/
bool gameLoadError() {
  fprintf(stderr, "Error when loading the game from file!\n");
  return false;
}

/**
 * @brief Prints the error in stderr if a solution file couldn't be created and
 *delete the loaded game
 *
 * @param g, the game loaded before the file was created
 * @return false
 **/
bool solFileError(game g) {
  if (g != NULL) delete_game(g);
  fprintf(stderr, "Error when attempting to create the solution file(s)!\n");
  return false;
}

//################################################################
//################## BRUTEFORCE SOLVER ###########################
//################################################################

bool bruteError() {
  fprintf(stderr, "Error in brute function !\n");
  return false;
}

int *bruteResolve(game g, int type, int n, int *sol, bool found,
                  const char *nameFile) {
  if (found) return sol;

  if (n == game_height(g) * game_width(g)) {
    bool result = is_game_over(g);
    if (result) {
      (*sol)++;
      if (type == 0) {
        found = true;
        char s[100];
        save_game(g, strcat(strcpy(s, nameFile), ".sol"));
        return sol;
      }
      if (type == 2) {
        char sol_num[10];
        sprintf(sol_num, "%d", *sol);
        char fNameCopy[100];
        save_game(g,
                  strcat(strcat(strcpy(fNameCopy, nameFile), ".sol"), sol_num));
      }
    }
    return sol;
  }

  int x = n % game_width(g);
  int y = n / game_width(g);
  // case of a segment:
  if (get_piece(g, x, y) == SEGMENT) {
    set_piece_current_dir(g, x, y, S);
    bruteResolve(g, type, n + 1, sol, found, nameFile);
    set_piece_current_dir(g, x, y, W);
    bruteResolve(g, type, n + 1, sol, found, nameFile);
  } else if (get_piece(g, x, y) == CROSS) {
    bruteResolve(g, type, n + 1, sol, found, nameFile);
  } else {
    // case of every pieces except segment and cross
    for (int dir = 0; dir < NB_DIR; dir++) {
      set_piece_current_dir(g, x, y, dir);
      bruteResolve(g, type, n + 1, sol, found, nameFile);
    }
  }
  return sol;
}

int brute(int type, game g,
          char *nameFile) {  // type : 0 =  find_one, 1 = nb_sol, 2= find_all
  if (type < 0 || type > 2) {
    return -1;
  }
  int sol = 0;
  return *bruteResolve(copy_game(g), type, 0, &sol, false, nameFile);
}
