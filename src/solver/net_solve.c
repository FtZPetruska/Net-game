#include "game.h"
#include "game_io.h"

//--------------------------------------------------------------------------------------
//                                Structures
typedef struct possibility_s *possibility;

// this structure is used as a chained list to save different dispositions of
// pieces.
struct possibility_s {
  uint32_t x;     // the coordinates of the corresponding cell in the game
  uint32_t y;     //
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

//--------------------------------------------------------------------------------------
//                                Error handler functions
//                         These functions are used by find_one/nb_sol/find_all
//                         to handle errors and properly leave the function, to
//                         tests if the arguments are valid, as well as to show
//                         how to use the solver
static void usage(char *argv[]);
static bool checkArgs(int argc, char *argv[]);

//--------------------------------------------------------------------------------------
//                                Main function

int main(int argc, char *argv[]) {
  if (!checkArgs(argc, argv)) usage(argv);

  bool status = true;

  if (strcmp(argv[1], "FIND_ONE") == 0) status = find_one(argv);

  if (strcmp(argv[1], "NB_SOL") == 0) status = nb_sol(argv);

  if (strcmp(argv[1], "FIND_ALL") == 0) status = find_all(argv);

  if (!status) {  // This tests whether the called function worked properly or
                  // not
    FPRINTF(stderr, "Error in %s!\n", argv[1]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

//--------------------------------------------------------------------------------------
//                                Error handler functions bodies

/**
 * @brief Prints the usage of the solver and kills the solver
 * @param argv, an array of the arguments
 **/
static void usage(char *argv[]) {
  FPRINTF(stderr,
          "%s FIND_ONE|NB_SOL|FIND_ALL <nom_fichier_pb> <prefix_fichier_sol>\n",
          argv[0]);
  exit(EXIT_FAILURE);
}

/**
 * @brief Checks how many arguments and their validity
 *
 * @param argc, the number of argument
 * @param argv, an array of the arguments
 * @return true if there are the right number of arguments and if they are all
 *valid
 **/
static bool checkArgs(int argc, char *argv[]) {
  if (argc != 4) return false;

  bool arg1 = false, arg2 = true, arg3 = true;

  if (strcmp(argv[1], "FIND_ONE") == 0 || strcmp(argv[1], "NB_SOL") == 0 ||
      strcmp(argv[1], "FIND_ALL") ==
          0) {  // strcmp returns 0 if the two strings are the same
    arg1 = true;
  }

  FILE *input_stream;
  FOPEN(input_stream, argv[2],
        "r");  // tests whether or not the file to solve is accessible
  if (!input_stream) {
    FPRINTF(stderr, "Couldn't open the requested file!\n");
    arg2 = false;
  } else
    FCLOSE(input_stream);

  FILE *output_stream;
  FOPEN(output_stream, argv[3],
        "w");  // tests if the name of the output file is valid
  if (!output_stream) {
    FPRINTF(stderr, "Couldn't create the new file!\n");
    arg3 = false;
  } else {
    FCLOSE(output_stream);
    remove(argv[3]);
  }

  return arg1 && arg2 && arg3;
}
