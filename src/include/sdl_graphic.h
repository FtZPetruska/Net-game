#ifndef MODEL_H
#define MODEL_H

#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "game.h"

typedef struct Env_t Env;

/* **************************************************************** */

#if defined(__ANDROID__)
#define PRINT(STR, ...)          \
  do {                           \
    SDL_Log(STR, ##__VA_ARGS__); \
  } while (0)
#define ERROR(STR, ...)          \
  do {                           \
    SDL_Log(STR, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);          \
  } while (0)
#else
#define PRINT(STR, ...)       \
  do {                        \
    printf(STR, __VA_ARGS__); \
  } while (0)
#define ERROR(STR, ...)                \
  do {                                 \
    fprintf(stderr, STR, __VA_ARGS__); \
    exit(EXIT_FAILURE);                \
  } while (0)
#endif

/* **************************************************************** */

#define APP_NAME "Net Game"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define DELAY 100
#define BORDER 10
#define BUTTON_BOTTOM_SPACE 100
#define GAME_BUTTONS_GAP 10
#define NB_BUTTONS 4
#define NB_SFX 2

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, game g);
void render(SDL_Window* win, SDL_Renderer* ren, Env* env);
void clean(SDL_Window* win, SDL_Renderer* ren, Env* env);
bool process(SDL_Window* win, Env* env, SDL_Event* e);

/* **************************************************************** */

#endif
