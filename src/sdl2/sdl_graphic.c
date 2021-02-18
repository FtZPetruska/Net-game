#include "sdl_graphic.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_mixer.h>  // required to use music
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_io.h"

/* **************************************************************** */

#define FONTSIZE 42

#ifdef __ANDROID__
#define FONT "font.ttf"
#define BACKGROUND "background.png"
#define BUTTON "button.png"
#define TEE_IMAGE "tee.png"
#define LEAF_IMAGE "leaf.png"
#define CORNER_IMAGE "corner.png"
#define SEGMENT_IMAGE "segment.png"
#define CROSS_IMAGE "cross.png"
#define MUSIC "music.ogg"
#define TURN_SFX1 "turn1.ogg"
#define TURN_SFX2 "turn2.ogg"

#else
#define FONT "assets/font.ttf"
#define BACKGROUND "assets/background.png"
#define BUTTON "assets/button.png"
#define TEE_IMAGE "assets/tee.png"
#define LEAF_IMAGE "assets/leaf.png"
#define CORNER_IMAGE "assets/corner.png"
#define SEGMENT_IMAGE "assets/segment.png"
#define CROSS_IMAGE "assets/cross.png"
#define MUSIC "assets/music.ogg"
#define TURN_SFX1 "assets/turn1.ogg"
#define TURN_SFX2 "assets/turn2.ogg"
#endif

/* **************************************************************** */

// Some macros to load the assets
#define LOAD_TEXTURE(texture, image)          \
  env->texture = IMG_LoadTexture(ren, image); \
  if (!env->texture) ERROR("IMG_LoadTexture: %s\n", image);

#define LOAD_TEXT(font, color, string, texture)               \
  surf = TTF_RenderText_Blended(font, string, color);         \
  env->texture = SDL_CreateTextureFromSurface(ren, surf);     \
  if (!env->texture) ERROR("TEXT_LoadTexture: %s\n", string); \
  SDL_FreeSurface(surf);

#define LOAD_SFX(chunk, sfx)     \
  env->chunk = Mix_LoadWAV(sfx); \
  if (!env->chunk) ERROR("AUDIO_MixerError: %s\n", sfx);

/* **************************************************************** */

void set_game_layout(SDL_Window* win, Env* env);
bool find_one_sdl(game g);  // Defined in src/solve_smart.c
game change_game(void);
bool sound_on;

int minimum(int a, int b) { return a < b ? a : b; }

/* **************************************************************** */

struct Env_t {
  game game;
  SDL_Texture* pieces[NB_PIECE_TYPE];
  SDL_Texture* background;
  SDL_Texture* button;
  SDL_Texture* button_text[NB_BUTTONS];
  SDL_Texture* label_win;
  Mix_Music* music;
  Mix_Chunk* turn_sfx[NB_SFX];
  int pos_x, pos_y, piece_size, win_w, win_h;
  bool win;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, game g) {
  Env* env = malloc(sizeof(struct Env_t));

  env->game = g;

  env->win = false;

  set_game_layout(win, env);

  LOAD_TEXTURE(background, BACKGROUND);
  LOAD_TEXTURE(pieces[LEAF], LEAF_IMAGE);
  LOAD_TEXTURE(pieces[SEGMENT], SEGMENT_IMAGE);
  LOAD_TEXTURE(pieces[CORNER], CORNER_IMAGE);
  LOAD_TEXTURE(pieces[TEE], TEE_IMAGE);
  LOAD_TEXTURE(pieces[CROSS], CROSS_IMAGE);
  LOAD_TEXTURE(button, BUTTON);

  TTF_Font* font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
  SDL_Color color = {255, 0, 0, 255};
  SDL_Surface* surf;

  LOAD_TEXT(font, color, "You win", label_win);
  LOAD_TEXT(font, color, "New game", button_text[0]);
  LOAD_TEXT(font, color, "Shuffle", button_text[1]);
  LOAD_TEXT(font, color, "Solve", button_text[2]);

#ifdef __ANDROID__
  LOAD_TEXT(font, color, "Mute",
            button_text[3]);  // Since saving makes the app crash, we'll make an
                              // exit button instead
#else
  LOAD_TEXT(font, color, "Save", button_text[3]);
#endif

  TTF_CloseFont(font);

  env->music = Mix_LoadMUS(MUSIC);
  if (!env->music) ERROR("AUDIO_MixerError: %s\n", MUSIC);

  LOAD_SFX(turn_sfx[0], TURN_SFX1);
  LOAD_SFX(turn_sfx[1], TURN_SFX2);

  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  SDL_Rect rect;

  int game_w, game_h;
  game_w = game_width(env->game);
  game_h = game_height(env->game);
  SDL_GetWindowSize(win, &env->win_w, &env->win_h);

  if (env->win_w > env->win_h) {
    rect.w = env->win_w;
    rect.h = rect.w;
  } else {
    rect.h = env->win_h;
    rect.w = rect.h;
  }
  rect.y = -(rect.h - env->win_h) / 2;
  rect.x = -(rect.w - env->win_w) / 2;

  SDL_RenderCopy(ren, env->background, NULL, &rect);

  rect.w = env->piece_size;
  rect.h = env->piece_size;
  piece piece;
  direction dir;
  for (int y = game_h - 1; y >= 0; y--) {
    for (int x = 0; x < game_w; x++) {
      piece = get_piece(env->game, x, y);
      dir = get_current_dir(env->game, x, y);
      rect.x = env->pos_x + x * env->piece_size;
      rect.y = env->pos_y + (game_h - (y + 1)) * env->piece_size;
      SDL_RenderCopyEx(ren, env->pieces[piece], NULL, &rect, (double)dir * 90,
                       NULL, SDL_FLIP_NONE);
    }
  }

  int button_width = (env->win_w - 2 * BORDER) / NB_BUTTONS;
  rect.w = button_width;
  rect.h = BUTTON_BOTTOM_SPACE;
  rect.y = env->win_h - BORDER - BUTTON_BOTTOM_SPACE;
  for (int i = 0; i < NB_BUTTONS; i++) {
    rect.x = BORDER + i * button_width;
    SDL_RenderCopy(ren, env->button, NULL, &rect);
  }
  rect.w *= 0.8;
  rect.h *= 0.8;
  for (int i = 0; i < NB_BUTTONS; i++) {
    rect.x = BORDER + i * button_width + (button_width * 0.2) / 2;
    SDL_RenderCopy(ren, env->button_text[i], NULL, &rect);
  }

  if (env->win) {
    SDL_QueryTexture(env->label_win, NULL, NULL, &rect.w, &rect.h);
    rect.x = (env->win_w - rect.w) / 2;
    rect.y = (env->win_h - rect.h) / 2;
    SDL_RenderCopy(ren, env->label_win, NULL, &rect);
  }
}

/* **************************************************************** */

bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e) {
  if (e->type == SDL_QUIT) {
    return true;
  }
  if (e->type == SDL_WINDOWEVENT &&
      e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    set_game_layout(win, env);

  int cursor_x, cursor_y;

  SDL_GetWindowSize(win, &env->win_w, &env->win_h);

#ifdef __ANDROID__
  if (e->type == SDL_FINGERDOWN) {
    cursor_x = e->tfinger.x * env->win_w; /* tfinger.x, normalized in [0..1] */
    cursor_y = e->tfinger.y * env->win_h;
#else

  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    cursor_x = mouse.x;
    cursor_y = mouse.y;

#endif

    if (cursor_y > env->win_h - BORDER - BUTTON_BOTTOM_SPACE) {
      if (cursor_x < BORDER || cursor_x > env->win_w - BORDER ||
          cursor_y > env->win_h - BORDER)
        return false;
      if (cursor_x < env->win_w / 4) {
        game new_game = change_game();
        if (new_game != NULL) {
          delete_game(env->game);
          env->game = new_game;
          env->win = false;
          set_game_layout(win, env);
        }
      } else if (cursor_x < env->win_w / 2) {
        shuffle_dir(env->game);
        env->win = false;
      } else if (cursor_x < env->win_w - env->win_w / 4)
        find_one_sdl(env->game);
      else
#ifdef __ANDROID__
          if (sound_on) {
        Mix_HaltMusic();
        sound_on = false;
      } else {
        Mix_PlayMusic(env->music, -1);
        sound_on = true;
      }
#else
        save_game(env->game, "save.sav");
#endif
    } else if (!env->win) {
      int x = env->pos_x, y = env->pos_y, size = env->piece_size;
      bool found_col = false, found_row = false;
      int piece_x, piece_y;
      for (piece_x = 0; piece_x < game_width(env->game); piece_x++) {
        if (cursor_x > piece_x * size + x &&
            cursor_x < (piece_x + 1) * size + x) {
          found_col = true;
          break;
        }
      }

      if (found_col) {
        for (piece_y = game_height(env->game) - 1; 0 <= piece_y; piece_y--) {
          if (cursor_y < (game_height(env->game) - (piece_y)) * size + y &&
              cursor_y > (game_height(env->game) - piece_y - 1) * size + y) {
            found_row = true;
            break;
          }
        }
      }

      if (found_row) {
        int turn;
#ifdef __ANDROID__
        turn = 1;
#else
        switch (e->button.button) {
          case SDL_BUTTON_LEFT:
            turn = 3;
            break;
          case SDL_BUTTON_MIDDLE:
            turn = 2;
            break;
          case SDL_BUTTON_RIGHT:
            turn = 1;
            break;
          default:
            turn = 0;
            break;
        }
#endif
        if (sound_on) Mix_PlayChannel(-1, env->turn_sfx[rand() % NB_SFX], 0);
        rotate_piece(env->game, piece_x, piece_y, turn);

        env->win = false;
        if (is_game_over(env->game)) {
          env->win = true;
        }
      }
    }
  }

  return false;
}

/* **************************************************************** */

void clean(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  if (!env) return;
  SDL_DestroyTexture(env->background);
  for (int i = 0; i < NB_PIECE_TYPE; i++) SDL_DestroyTexture(env->pieces[i]);
  SDL_DestroyTexture(env->button);
  for (int i = 0; i < NB_BUTTONS; i++) SDL_DestroyTexture(env->button_text[i]);
  SDL_DestroyTexture(env->label_win);

  Mix_FreeMusic(env->music);
  for (int i = 0; i < NB_SFX; i++) Mix_FreeChunk(env->turn_sfx[i]);

  free(env);
}

/* **************************************************************** */

void set_game_layout(SDL_Window* win, Env* env) {
  int game_w, game_h;
  SDL_GetWindowSize(win, &env->win_w, &env->win_h);
  game_w = game_width(env->game);
  game_h = game_height(env->game);
  env->win_w = env->win_w - 2 * BORDER;
  env->win_h = env->win_h - 3 * BORDER - BUTTON_BOTTOM_SPACE;
  env->piece_size = minimum(env->win_w / game_w, env->win_h / game_h);
  // avoid 1 px gap between pieces, because of piece rotation algorithm:
  if (env->piece_size % 2 == 1) env->piece_size += 1;
  int offset_x = (env->piece_size * game_w) / 2;
  int offset_y = (env->piece_size * game_h) / 2;
  env->pos_x = env->win_w / 2 - offset_x + BORDER;
  env->pos_y = env->win_h / 2 - offset_y + BORDER;
}

int open_graphic(game g) {
  /* initialize SDL2 and some extensions */
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    ERROR("Error: SDL_Init VIDEO (%s)", SDL_GetError());
  if (IMG_Init(IMG_INIT_PNG & IMG_INIT_PNG) != IMG_INIT_PNG)
    ERROR("Error: IMG_Init PNG (%s)", SDL_GetError());
  if (TTF_Init() != 0) ERROR("Error: TTF_Init (%s)", SDL_GetError());

  if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    ERROR("Error: SDL_Mixer init %s\n", Mix_GetError());

  /* create window and renderer */
  SDL_Window* win = SDL_CreateWindow(
      APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!win) ERROR("Error: SDL_CreateWindow (%s)", SDL_GetError());
  SDL_Renderer* ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!ren) ERROR("Error: SDL_CreateRenderer (%s)", SDL_GetError());

  /* initialize your environment */
  Env* env = init(win, ren, g);

  /* starts playing music */
  Mix_PlayMusic(env->music, -1);
  sound_on = true;

  /* main render loop */
  SDL_Event e;
  bool quit = false;
  while (!quit) {
    /* manage events */
    while (SDL_PollEvent(&e)) {
      /* process your events */
      quit = process(win, ren, env, &e);
      if (quit) break;
    }

    /* background in gray */
    SDL_SetRenderDrawColor(ren, 0xA0, 0xA0, 0xA0, 0xFF);
    SDL_RenderClear(ren);

    /* render all what you want */
    render(win, ren, env);
    SDL_RenderPresent(ren);
    SDL_Delay(DELAY);
  }

  /* clean your environment */
  clean(win, ren, env);

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}
