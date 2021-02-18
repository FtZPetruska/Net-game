#include "game_rand.h"

#include <assert.h>
#include <stdlib.h>

typedef struct point_t {
  int x, y;
} point;

static point translate(point origin, point vector) {
  return (point){origin.x + vector.x, origin.y + vector.y};
}

static point get_random_point_on_board(cgame board) {
  int x = rand() % game_width(board);
  int y = rand() % game_height(board);
  return (point){x, y};
}
typedef struct edge_t {
  point point;
  direction piece_direction;
} edge;
typedef struct edge_links_t {
  bool links[NB_DIR];
} edge_links;

typedef struct board_element_t {
  piece piece_type;
  direction piece_direction;
} board_element;

int get_edge_links_count(edge_links edge_links) {
  int result = 0;
  for (int i = 0; i < NB_DIR; i++)
    if (edge_links.links[i]) result++;
  return result;
}

static direction get_random_dir() { return (direction)(rand() % NB_DIR); }

static bool is_filled(cgame board) {
  for (int x = 0; x < game_width(board); x++)
    for (int y = 0; y < game_height(board); y++)
      if (get_piece(board, x, y) == EMPTY) return false;
  return true;
}

static bool is_point_in_board_bounds(cgame board, point point) {
  return (0 <= point.x && point.x < game_width(board)) &&
         (0 <= point.y && point.y < game_height(board));
}

static point get_neighbour_point(cgame board, edge edge) {
  const point delta[NB_DIR] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  point neighbor_point = translate(edge.point, delta[edge.piece_direction]);
  if (is_wrapping(board)) {
    neighbor_point.x = neighbor_point.x % game_width(board);
    neighbor_point.y = neighbor_point.y % game_height(board);
  }
  return neighbor_point;
}

static edge_links get_edge_links_from_board_element(board_element element) {
  edge_links result = {{false, false, false, false}};
  for (direction dir = 0; dir < NB_DIR; dir++) {
    if (is_edge(element.piece_type, element.piece_direction, dir)) {
      result.links[dir] = true;
    }
  }
  return result;
}

static board_element get_board_element_from_edge_links(edge_links edge_links) {
  piece new_piece;
  direction new_direction;
  switch (get_edge_links_count(edge_links)) {
    case 0:
      new_piece = EMPTY;
      new_direction = N;
      break;
    case 1:
      new_piece = LEAF;
      for (direction orientation = 0; orientation < NB_DIR; orientation++) {
        if (edge_links.links[orientation]) new_direction = orientation;
      }
      break;
    case 2:
      for (direction orientation = 0; orientation < NB_DIR; orientation++) {
        if (edge_links.links[orientation] &&
            edge_links.links[(orientation + 1) % NB_DIR]) {
          new_piece = CORNER;
          new_direction = orientation;
        }
        if (edge_links.links[orientation] &&
            edge_links.links[(orientation + 2) % NB_DIR]) {
          new_piece = SEGMENT;
          new_direction = orientation;
        }
      }
      break;
    case 3:
      new_piece = TEE;
      for (direction orientation = 0; orientation < NB_DIR; orientation++)
        if (!edge_links.links[orientation])
          new_direction = opposite_dir(orientation);
      break;
    case 4:
      new_piece = CROSS;
      new_direction = N;
      break;
    default:
      break;
  }
  return (board_element){new_piece, new_direction};
}

static void add_half_edge(game board, edge new_edge) {
  board_element old_element = {
      get_piece(board, new_edge.point.x, new_edge.point.y),
      get_current_dir(board, new_edge.point.x, new_edge.point.y)};
  edge_links piece_edge_links = get_edge_links_from_board_element(old_element);
  piece_edge_links.links[new_edge.piece_direction] = true;
  board_element new_element =
      get_board_element_from_edge_links(piece_edge_links);
  set_piece(board, new_edge.point.x, new_edge.point.y, new_element.piece_type,
            new_element.piece_direction);
}

static void add_edge(game board, edge new_edge) {
  edge neighbour_edge = {get_neighbour_point(board, new_edge),
                         opposite_dir(new_edge.piece_direction)};
  assert(is_point_in_board_bounds(board, neighbour_edge.point));
  add_half_edge(board, new_edge);
  add_half_edge(board, neighbour_edge);
}

static bool is_valid_candidate(cgame board, edge edge, bool allow_cross) {
  point neighbough_point = get_neighbour_point(board, edge);
  if (!is_point_in_board_bounds(board, neighbough_point)) return false;
  return (get_piece(board, edge.point.x, edge.point.y) != EMPTY &&
          get_piece(board, neighbough_point.x, neighbough_point.y) == EMPTY &&
          (allow_cross || get_piece(board, edge.point.x, edge.point.y) != TEE));
}

static edge get_random_candidate(cgame board, bool allow_cross) {
  assert(!is_filled(board));
  edge new_edge = {get_random_point_on_board(board), get_random_dir()};
  while (!is_valid_candidate(board, new_edge, allow_cross)) {
    new_edge.point = get_random_point_on_board(board);
    new_edge.piece_direction = get_random_dir();
  }
  return new_edge;
}

game random_game_ext(int width, int height, bool is_swap, bool allow_cross) {
  assert(width >= 3);
  assert(height >= 3);

  game new_board = new_game_empty_ext(width, height, is_swap);

  edge initial_edge = {{width / 2, height / 2}, get_random_dir()};

  add_edge(new_board, initial_edge);
  while (!is_filled(new_board)) {
    edge new_edge = get_random_candidate(new_board, allow_cross);
    add_edge(new_board, new_edge);
  }
  assert(is_game_over(new_board));
  shuffle_dir(new_board);
  return new_board;
}
