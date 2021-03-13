#ifndef GAME_IO_H
#define GAME_IO_H
#include "game.h"

/**
 * @file game_io.h
 *
 * @brief This file provides functions to load or save a game.
 *
 * The format used for save files is the following:
 * The first line contains respectively the width, the height and the wrapping
 *boolean. On subsequent lines the game board is represented with pairs of
 *PIECE;DIRECTION.
 **/

/**
 * @brief Creates a game by loading its description in a file
 *
 * @param filename
 * @return the loaded game
 **/
game load_game(char *filename);

/**
 * @brief Save a game in a file
 *
 * @param g game to save
 * @param filename output file
 **/
void save_game(cgame board, char *filename);

#endif  // GAME_IO_H
