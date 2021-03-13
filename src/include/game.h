#ifndef __GAME_H__
#define __GAME_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "cross_io.h"

#define DEFAULT_SIZE 5

#define MIN_GAME_WIDTH (uint16_t)3
#define MIN_GAME_HEIGHT (uint16_t)3

#define MAX_GAME_WIDTH (uint16_t)512
#define MAX_GAME_HEIGHT (uint16_t)512

/**
 * @file game.h
 * @brief This file describes the programming interface of a puzzle
 * game, named 'Net'.
 */

/**
 * @mainpage
 *
 * This game was originally invented by David Millar. A demo of this game is
 * provided in the Simon Tatham's Portable Puzzle Collection, under the name
 * <a
href="https://www.chiark.greenend.org.uk/~sgtatham/puzzles/js/net.html">'Net'</a>.
 *
 * The goal of the game is to join up the pieces on a grid into a single
 * connected network with no loops. This can be achieved by rotating the squares
of
 * the grid in a clockwise manner.
 *
 * The squares of the grid are denoted by (x,y) coordinates, (0,0) corresponding
to
 * the bottom left corner, and the first coordinate indicating the column.
 * *
 * Here is an example of a game and its solution.
 *
 @verbatim

 | Example of game

|  ┘ ^ < └ v  |  ┌ < > ┐ v  |
|  ┬ ┤ ┴ ├ ├  |  ├ ┬ ┬ ┴ ┤  |
|  > ^ ┤ < -  |  ^ ^ ├ < |  |
|  v ┬ ┴ ┘ |  |  > ┬ ┴ ┐ |  |
|  > ┤ v > v  |  > ┴ < ^ ^  |
|             |             |
|    start    |   solution  |

 @endverbatim
 * Observe the four types of pieces in their default direction (North):
 * leaf (^), segment (|) corner (└) and tee (┴).
 * In the V2, A FIFTH PIECE IS PRESENT, CROSS (+).
 *
 * Note that a leaf directed West (<) cannot be connected to its right
 * to another piece . This is also true for a segment directed North/South (|),
 * a tee to West (┤) and a corner directed South (┐) or West (┘).
 *
 * Observe that in the solution all the pieces are connected in single a
network.
 * Moreover, in the V2 THE GRID MAY BE "WRAPPING", meaning that pieces on the
edge of
 * the grid connect on the opposite edge (connections come back on the opposite
side of the grid).
 * See wrapping game examples in Simon Tatham's Portable Puzzle Collection.
**/

/**
 * @brief The possible pieces
 **/
typedef enum piece_e {
  EMPTY = -1,
  LEAF = 0,
  SEGMENT = 1,
  CORNER = 2,
  TEE = 3,
  CROSS = 4
} piece;

#define NB_PIECE_TYPE 5

/**
 * @brief The four cardinal directions
 **/
typedef enum direction_e { N = 0, E = 1, S = 2, W = 3 } direction;

#define NB_DIR 4

/**
 * @brief The structure pointer that stores the game
 **/
typedef struct game_s *game;

/**
 * @brief The structure constant pointer that stores the game
 * That means that it is not possible to modify the game using this pointer.
 * See also: http://www.geeksforgeeks.org/const-qualifier-in-c/
 * See also this more technical discussion:
 *http://stackoverflow.com/questions/8504411/typedef-pointer-const-weirdness
 **/
typedef const struct game_s *cgame;

/**
 * @brief Creates an empty game (DEFAULT_SIZE x DEFAULT_SIZE) with squares set
 *to empty and directions set to North
 * @return the newly created game, NULL in case of error
 **/
game new_game_empty();

/**
 * @brief Creates an empty game with squares set to empty and directions set to
 *North
 * @param width the width of the grid for the empty game (number of columns)
 * @param height the height of the grid for the empty game (number of rows)
 * @param wrapping a boolean indicating whether the grid is "wrapping"
 * @return the newly created game, NULL in case of error
 **/
game new_game_empty_ext(uint16_t width, uint16_t height, bool wrapping);

/**
 * @brief Creates a new game having a square grid of DEFAULT_SIZE x DEFAULT_SIZE
 * @param pieces the pieces to fill the grid given row by row starting from the
 *bottom of the grid
 * @param initial_directions the directions in which the pieces are oriented
 *initially
 * @return the newly created game, NULL in case of error
 **/
game new_game(const piece *pieces, const direction *initial_directions);

/**
 * @brief Creates a new game having a grid of width x height
 * @param width the width of the new grid (number of columns)
 * @param height the height of the new grid (number of rows)
 * @param pieces the pieces to fill the grid given row by row starting from the
 *bottom of the grid
 * @param initial_directions the directions in which the pieces are oriented
 *initially
 * @param wrapping a boolean indicating whether the grid is "wrapping"
 * @return the newly created game, NULL in case of error
 **/
game new_game_ext(uint16_t width, uint16_t height, const piece *pieces,
                  const direction *initial_directions, bool wrapping);

/**
 * @brief Initialises a grid square to a given piece and orientation
 * @param board the game to be modified
 * @param x the x coordinate (the column) of the square on the grid
 * @param y the y coordinate (the row) of the square on the grid
 * @param piece the piece to be put in the grid square
 * @param dir the initial orientation of the piece that is set
 **/
void set_piece(game board, uint16_t x, uint16_t y, piece new_piece,
               direction new_direction);

/**
 * @brief Returns whether the grid is defined as being "wrapping" or not
 * @param board a constant pointer on the game to be tested
 * @return true if the grid is wrapping, false otherwise or in case of error
 **/
bool is_wrapping(cgame board);

/**
 * @brief Shuffles the current orientations of the pieces
 * @param board the game to be shuffled
 **/
void shuffle_direction(game board);

/**
 * @brief Returns the number of rows on the grid
 * @param board the game
 * @return the height of the game, 0 in case of error
 **/
uint16_t game_height(cgame board);

/**
 * @brief Returns the number of columns on the game
 * @param board the game
 * @return the width of the game, 0 in case of error
 **/
uint16_t game_width(cgame board);

/**
 * @brief Rotates a piece once clockwise
 * @param board the game
 * @param x the x coordinate of the piece on the grid
 * @param y the y coordinate of the piece on the grid
 **/
void rotate_piece_one(game board, uint16_t x, uint16_t y);

/**
 * @brief Rotates a piece several times clockwise
 * @param board the game
 * @param x the x coordinate of the piece on the grid
 * @param y the y coordinate of the piece on the grid
 * @param nb_ccw_quarter_turn number of clockwise turns to be applied
 **/
void rotate_piece(game board, uint16_t x, uint16_t y,
                  int32_t nb_cw_quarter_turn);

/**
 * @brief Sets the current direction of the piece located at (x,y)
 * @param board the game to be modified
 * @param x the column (x coordinate) of the piece on the grid
 * @param y the row (y coordinate) of the piece on the grid
 * @param dir the current direction to be set for the piece located at (x,y)
 **/
void set_piece_current_direction(game board, uint16_t x, uint16_t y,
                                 direction new_direction);

/**
 * @brief Tests whether a piece on a given position is oriented so as it could
 *be connected in the given direction
 * @param g a constant pointer on the game
 * @param x the column (x coordinate) of the piece on the grid
 * @param y the row (y coordinate) of the piece on the grid
 * @param dir the direction in which to test whether the piece could be
 *connected
 * @return true if the piece on (x,y) could be connected in the direction dir,
 *false otherwise or in case of error
 **/
bool is_edge_coordinates(cgame board, uint16_t x, uint16_t y, direction dir);

/**
 * @brief Tests whether a piece with a specific orientation could be connected
 *in the given direction
 * @param piece the type of the piece
 * @param orientation the orientation of the piece
 * @param dir the direction in which to test whether the piece could be
 *connected
 * @return true if the piece could be connected in the direction dir, false
 *otherwise, false in case of error
 **/

bool is_edge(piece test_piece, direction orientation, direction dir);

/**
 * @brief Computes the opposite of a given direction
 * @param dir the direction
 * @return the opposite direction of dir, -1 in case of error
 **/

direction opposite_direction(direction dir);

/**
 * @brief Clones the game source_board
 * @param source_board a constant pointer on the game to clone
 * @return the clone of source_board, NULL in case of error
 **/
game copy_game(cgame source_board);

/**
 * @brief Destroys the game and frees the allocated memory
 * @param board the game to destroy
 **/
void delete_game(game board);

/**
 * @brief Gets the piece located at (x,y) square on the grid
 * @param board the game we consider
 * @param x the x coordinate of the square
 * @param y the y coordinate of the square
 * @return the piece of the board, -2 in case of error
 **/
piece get_piece(cgame board, uint16_t x, uint16_t y);

/**
 * @brief Gets the current orientation of a square on the grid
 * @param board a constant pointer on the game we consider
 * @param x the x coordinate of the square
 * @param y the y coordinate of the square
 * @return the current orientation, -1 in case of error
 **/
direction get_current_direction(cgame board, uint16_t x, uint16_t y);

/**
 * @brief Tests if the game is over (that is the grid is filled according to the
 *requirements)
 * @param board the board to test
 * @return true if all the constraints are satisfied, false in case of error
 **/
bool is_game_over(cgame board);

/**
 * @brief Restarts a game by reinitialising all the current directions to the
 *initial directions
 * @param board the board to restart
 **/
void restart_game(game board);

#endif  // __GAME_H__
