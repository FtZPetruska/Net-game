#ifndef GAME_RAND_H
#define GAME_RAND_H
#include "game.h"

/**
 * @brief Creates a valid game randomly
 * @param width the width of the board for the game (number of columns)
 * @param height the height of the board for the game (number of rows)
 * @param swap a boolean indicating whether the board is "wrapping"
 * @param allow_cross a boolean indicating whether the board can contain CROSS
 *pieces
 * @return the randomly generated game
 **/
game random_game_ext(uint16_t width, uint16_t height, bool swap, bool allow_cross);

#endif  // GAME_RAND_H
