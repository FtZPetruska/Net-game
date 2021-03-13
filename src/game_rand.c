#include "game_rand.h"

/**
 * @brief Structure for a 2D point
 */
typedef struct point_t {
  int32_t x; /**< x coordinate of the point. */
  int32_t y; /**< y coordinate of the point. */
} point;

/**
 * @brief translates a point by a given vector
 *
 * @param origin, the point to be translated
 * @param vector, the vector of translation
 * @return the translated origin
 */
static point translate(point origin, point vector) {
  return (point){origin.x + vector.x, origin.y + vector.y};
}

/**
 * @brief generates a random point in the game bounds
 *
 * @param board, the game in which we want a random point
 * @return a point within the game bounds
 */
static point get_random_point_on_board(cgame board) {
  int32_t x = rand() % game_width(board);
  int32_t y = rand() % game_height(board);
  return (point){x, y};
}

/**
 * @brief Structure to manipulate position and direction.
 */
typedef struct edge_t {
  point point;               /**< Position of the edge. */
  direction piece_direction; /**< Direction of the edge. */
} edge;

/**
 * @brief Structure to keep track of all the links of an edge.
 */
typedef struct edge_links_t {
  bool links[NB_DIR]; /**< Array for each direction. */
} edge_links;

/**
 * @brief Structure that holds the piece and the required direction to complete
 * the game.
 */
typedef struct board_element_t {
  piece piece_type;          /**< Type of the piece. */
  direction piece_direction; /**< Direction of the piece. */
} board_element;

/**
 * @brief get the total of links of an edge_links
 *
 * @param links, the object to count
 * @return the amount of links set to true
 */
uint8_t get_edge_links_count(edge_links links) {
  uint8_t result = 0;
  for (uint8_t i = 0; i < NB_DIR; i++)
    if (links.links[i]) result++;
  return result;
}

/**
 * @brief generates a random direction
 * @return a random direction
 */
static direction get_random_dir() { return (direction)(rand() % NB_DIR); }

static bool is_filled(cgame board) {
  for (uint16_t x = 0; x < game_width(board); x++)
    for (uint16_t y = 0; y < game_height(board); y++)
      if (get_piece(board, x, y) == EMPTY) return false;
  return true;
}

/**
 * @brief checks if a point is within the bounds of a game
 *
 * @param board, the game to test against
 * @param test_point, the point to test
 * @return true if the point is in the game, false otherwise
 */
static bool is_point_in_board_bounds(cgame board, point test_point) {
  int32_t width = (int32_t)game_width(board);
  int32_t height = (int32_t)game_height(board);
  return (0 <= test_point.x && test_point.x < width) &&
         (0 <= test_point.y && test_point.y < height);
}

/**
 * @brief finds a neighbor point of an edge
 *
 * @param board, the game we're generating
 * @param current_edge, the edge we want to get the neighbor of
 * @return the neighbor point of the current edge
 */
static point get_neighbor_point(cgame board, edge current_edge) {
  const point delta[NB_DIR] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  point neighbor_point =
      translate(current_edge.point, delta[current_edge.piece_direction]);
  if (is_wrapping(board)) {
    neighbor_point.x = neighbor_point.x % game_width(board);
    neighbor_point.y = neighbor_point.y % game_height(board);
  }
  return neighbor_point;
}

/**
 * @brief converts a board element into an edge links
 *
 * @param element, the element to convert
 * @return an edge link with true in each direction the element has an edge
 */
static edge_links get_edge_links_from_board_element(board_element element) {
  edge_links result = {{false, false, false, false}};
  for (direction dir = 0; dir < NB_DIR; dir++) {
    if (is_edge(element.piece_type, element.piece_direction, dir)) {
      result.links[dir] = true;
    }
  }
  return result;
}

/**
 * @brief converts an edge links into a board element
 *
 * @param links, the edge links to convert
 * @return a board element with the piece and direction of the given edge_link
 */
static board_element get_board_element_from_edge_links(edge_links links) {
  piece new_piece;
  direction new_direction;
  switch (get_edge_links_count(links)) {
    case 0:
      new_piece = EMPTY;
      new_direction = N;
      break;
    case 1:
      new_piece = LEAF;
      for (direction orientation = 0; orientation < NB_DIR; orientation++) {
        if (links.links[orientation]) new_direction = orientation;
      }
      break;
    case 2:
      for (direction orientation = 0; orientation < NB_DIR; orientation++) {
        if (links.links[orientation] &&
            links.links[(orientation + 1) % NB_DIR]) {
          new_piece = CORNER;
          new_direction = orientation;
        }
        if (links.links[orientation] &&
            links.links[(orientation + 2) % NB_DIR]) {
          new_piece = SEGMENT;
          new_direction = orientation;
        }
      }
      break;
    case 3:
      new_piece = TEE;
      for (direction orientation = 0; orientation < NB_DIR; orientation++)
        if (!links.links[orientation])
          new_direction = opposite_direction(orientation);
      break;
    case 4:
      new_piece = CROSS;
      new_direction = N;
      break;
    default:
      new_piece = EMPTY;
      new_direction = N;
      break;
  }
  return (board_element){new_piece, new_direction};
}

/**
 * @brief adds one side of an edge
 *
 * @param board, the board we're generating
 * @param new_edge, the edge to be added
 */
static void add_half_edge(game board, edge new_edge) {
  uint16_t new_edge_x = (uint16_t)new_edge.point.x;
  uint16_t new_edge_y = (uint16_t)new_edge.point.y;
  board_element old_element = {
      get_piece(board, new_edge_x, new_edge_y),
      get_current_direction(board, new_edge_x, new_edge_y)};
  edge_links piece_edge_links = get_edge_links_from_board_element(old_element);
  piece_edge_links.links[new_edge.piece_direction] = true;
  board_element new_element =
      get_board_element_from_edge_links(piece_edge_links);
  set_piece(board, new_edge_x, new_edge_y, new_element.piece_type,
            new_element.piece_direction);
}

/**
 * @brief adds a new edge to the board (finds the neighbor's edge and calls
 * add_half_edge on both)
 *
 * @param board, the board we're generating
 * @param new_edge, the edge to be added
 */
static void add_edge(game board, edge new_edge) {
  edge neighbor_edge = {get_neighbor_point(board, new_edge),
                        opposite_direction(new_edge.piece_direction)};
  if (!is_point_in_board_bounds(board, neighbor_edge.point)) {
    FPRINTF(stderr, "Error: add_edge, the given point is out of bound.\n");
    return;
  }
  add_half_edge(board, new_edge);
  add_half_edge(board, neighbor_edge);
}

/**
 * @brief checks is an edge is a valid candidate
 *
 * @param board, the board we're generating
 * @param current_edge, the edge to test
 * @param allow_cross, a bool indicating whether or not CROSS pieces are allowed
 * @return true if all the constraints are satisfied, false otherwise
 */
static bool is_valid_candidate(cgame board, edge current_edge,
                               bool allow_cross) {
  uint16_t current_edge_x = (uint16_t)current_edge.point.x;
  uint16_t current_edge_y = (uint16_t)current_edge.point.y;

  point neighbor_point = get_neighbor_point(board, current_edge);
  uint16_t neighbor_x = (uint16_t)neighbor_point.x;
  uint16_t neighbor_y = (uint16_t)neighbor_point.y;
  if (!is_point_in_board_bounds(board, neighbor_point)) return false;
  return (
      get_piece(board, current_edge_x, current_edge_y) != EMPTY &&
      get_piece(board, neighbor_x, neighbor_y) == EMPTY &&
      (allow_cross || get_piece(board, current_edge_x, current_edge_y) != TEE));
}

static edge get_random_candidate(cgame board, bool allow_cross) {
  if (is_filled(board)) {
    FPRINTF(stderr, "Error: get_random_candidate, board is already filled.\n");
    return (edge){{-1, -1}, N};
  }
  edge new_edge = {get_random_point_on_board(board), get_random_dir()};
  while (!is_valid_candidate(board, new_edge, allow_cross)) {
    new_edge.point = get_random_point_on_board(board);
    new_edge.piece_direction = get_random_dir();
  }
  return new_edge;
}

game random_game_ext(uint16_t width, uint16_t height, bool swap,
                     bool allow_cross) {
  if (width < MIN_GAME_WIDTH || MAX_GAME_WIDTH < width) {
    FPRINTF(stderr,
            "Error: new_game_empty_ext, given width %hu is out of the [%hu, "
            "%hu] range.\n",
            width, MIN_GAME_WIDTH, MAX_GAME_WIDTH);
    return NULL;
  }
  if (height < MIN_GAME_HEIGHT || MAX_GAME_HEIGHT < height) {
    FPRINTF(stderr,
            "Error: new_game_empty_ext, given height %hu is out of the [%hu, "
            "%hu] range.\n",
            height, MIN_GAME_HEIGHT, MAX_GAME_HEIGHT);
    return NULL;
  }

  game new_board = new_game_empty_ext(width, height, swap);

  edge initial_edge = {{width / 2, height / 2}, get_random_dir()};

  add_edge(new_board, initial_edge);
  while (!is_filled(new_board)) {
    edge new_edge = get_random_candidate(new_board, allow_cross);
    add_edge(new_board, new_edge);
  }
  if (!is_game_over(new_board)) {
    FPRINTF(stderr, "Error: random_game_ext, the generated game is invalid.\n");
    delete_game(new_board);
    return NULL;
  }
  shuffle_direction(new_board);
  return new_board;
}
