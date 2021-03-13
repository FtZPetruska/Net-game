#include <time.h>

#include "game.h"
#include "game_io.h"
#include "game_rand.h"

#ifdef __ANDROID__
#include <SDL.h>  // Required when using Android
#endif

game SDL_game;
/* **************************************************************** */

game change_game(void);
int open_graphic(game SDL_game);
static void usage();

/* **************************************************************** */

int main(int argc, char* argv[]) {
  if (argc > 2) {
    FPRINTF(stderr, "Too many arguments!\n");
    usage();
    return EXIT_FAILURE;
  }
  srand((uint32_t)time(0));

  // Loads a game file
  if (argc == 2) {
    SDL_game = load_game(argv[1]);
    if (!SDL_game) {
      FPRINTF(stderr, "Error when loading game file\n");
      usage();
      return EXIT_FAILURE;
    }
  } else {  // Generates a random 5x5 game with crosses and no wrapping
    SDL_game = random_game_ext(5, 5, false, true);
    if (!SDL_game) {
      FPRINTF(stderr, "Error when creating default game\n");
      usage();
      return EXIT_FAILURE;
    }
  }
  open_graphic(SDL_game);

  delete_game(SDL_game);
  return EXIT_SUCCESS;
}

static void usage() { FPRINTF(stderr, "./net_sdl <savefile>"); }

game change_game(void) {
#ifdef __ANDROID__
  game new_game = random_game_ext(
      (uint16_t)rand() % 10 + 3, (uint16_t)rand() % 17 + 3, false,
      true);  // Generates a random game between 3x3 and 13x20
#else
  game new_game = random_game_ext(
      (uint16_t)rand() % 17 + 3, (uint16_t)rand() % 17 + 3, false,
      true);  // Generates a random game between 3x3 and 20x20
#endif
  if (!new_game) return NULL;
  SDL_game = new_game;
  return new_game;
}
