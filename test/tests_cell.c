#include <stdio.h>
#include <string.h>

#include "cell.h"

int test_alloc_cell() {
  cell new_cell = alloc_cell();

  if (!new_cell) {
    FPRINTF(stderr,
            "Error: test_alloc_cell, new cell has not been allocated.\n");
  }

  if (get_piece_cell(new_cell) != EMPTY) {
    FPRINTF(stderr,
            "Error: test_alloc_cell, new cell was initialized with %d instead "
            "of %d.\n",
            get_piece_cell(new_cell), EMPTY);
    free_cell(new_cell);
    return EXIT_FAILURE;
  }

  if (get_current_direction_cell(new_cell) != N) {
    FPRINTF(stderr,
            "Error: test_alloc_cell, new cell has current direction %u instead "
            "of initial %u",
            get_current_direction_cell(new_cell), N);
    free_cell(new_cell);
    return EXIT_FAILURE;
  }

  if (get_default_direction_cell(new_cell) != N) {
    FPRINTF(stderr,
            "Error: test_alloc_cell, new cell has default direction %u instead "
            "of initial %u",
            get_default_direction_cell(new_cell), N);
    free_cell(new_cell);
    return EXIT_FAILURE;
  }

  if (!is_out_of_bounds_cell(get_left_cell(new_cell))) {
    FPRINTF(
        stderr,
        "Error: test_alloc_cell, new cell has an invalid left neighbour.\n");
    return EXIT_FAILURE;
    free_cell(new_cell);
  }

  if (!is_out_of_bounds_cell(get_right_cell(new_cell))) {
    FPRINTF(
        stderr,
        "Error: test_alloc_cell, new cell has an invalid right neighbour.\n");
    return EXIT_FAILURE;
    free_cell(new_cell);
  }

  if (!is_out_of_bounds_cell(get_top_cell(new_cell))) {
    FPRINTF(stderr,
            "Error: test_alloc_cell, new cell has an invalid top neighbour.\n");
    return EXIT_FAILURE;
    free_cell(new_cell);
  }

  if (!is_out_of_bounds_cell(get_bottom_cell(new_cell))) {
    FPRINTF(
        stderr,
        "Error: test_alloc_cell, new cell has an invalid bottom neighbour.\n");
    return EXIT_FAILURE;
    free_cell(new_cell);
  }

  free_cell(new_cell);
  return EXIT_SUCCESS;
}

int test_free_cell_valid() {
  cell new_cell = alloc_cell();
  free_cell(new_cell);
  return EXIT_SUCCESS;
}

int test_free_cell_null_cell() {
  free_cell(NULL);
  return EXIT_SUCCESS;
}

int test_get_right_cell_valid() {
  cell left_cell = alloc_cell();
  cell right_cell = alloc_cell();
  set_right_cell(left_cell, right_cell);

  if (get_right_cell(left_cell) != right_cell) {
    FPRINTF(stderr,
            "Error: test_get_right_cell_valid, the right neighbour is "
            "different from right_cell.\n");
    free_cell(left_cell);
    free_cell(right_cell);
    return EXIT_FAILURE;
  }

  free_cell(left_cell);
  free_cell(right_cell);
  return EXIT_SUCCESS;
}

int test_get_right_cell_oob_neighbour() {
  cell left_cell = alloc_cell();
  cell right_cell = make_out_of_bounds_cell();
  set_right_cell(left_cell, right_cell);

  if (!is_out_of_bounds_cell(get_right_cell(left_cell))) {
    FPRINTF(stderr,
            "Error: test_get_right_cell_oob_neighbour, the right neighbour is "
            "not out of bounds.\n");
    free_cell(left_cell);
    return EXIT_FAILURE;
  }

  free_cell(left_cell);
  return EXIT_SUCCESS;
}

int test_get_right_cell_null_cell() {
  cell fake_cell = make_out_of_bounds_cell();
  if (get_right_cell(fake_cell) != NULL) {
    FPRINTF(stderr,
            "Error: test_get_right_cell_null_cell, function returned something "
            "other than NULL on an invalid cell");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int test_get_left_cell_valid() {
  cell left_cell = alloc_cell();
  cell right_cell = alloc_cell();
  set_left_cell(right_cell, left_cell);

  if (get_left_cell(right_cell) != left_cell) {
    FPRINTF(stderr,
            "Error: test_get_left_cell_valid, the left neighbour is "
            "different from left_cell.\n");
    free_cell(left_cell);
    free_cell(right_cell);
    return EXIT_FAILURE;
  }

  free_cell(left_cell);
  free_cell(right_cell);
  return EXIT_SUCCESS;
}

int test_get_left_cell_oob_neighbour() {
  cell left_cell = make_out_of_bounds_cell();
  cell right_cell = alloc_cell();
  set_left_cell(right_cell, left_cell);

  if (!is_out_of_bounds_cell(get_left_cell(right_cell))) {
    FPRINTF(stderr,
            "Error: test_get_left_cell_oob_neighbour, the left neighbour is "
            "not out of bounds.\n");
    free_cell(right_cell);
    return EXIT_FAILURE;
  }

  free_cell(right_cell);
  return EXIT_SUCCESS;
}

int test_get_left_cell_null_cell() {
  cell fake_cell = make_out_of_bounds_cell();
  if (get_left_cell(fake_cell) != NULL) {
    FPRINTF(stderr,
            "Error: test_get_left_cell_null_cell, function returned something "
            "other than NULL on an invalid cell");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int test_get_top_cell_valid() {
  cell bottom_cell = alloc_cell();
  cell top_cell = alloc_cell();
  set_top_cell(bottom_cell, top_cell);

  if (get_top_cell(bottom_cell) != top_cell) {
    FPRINTF(stderr,
            "Error: test_get_top_cell_valid, the top neighbour is "
            "different from top_cell.\n");
    free_cell(bottom_cell);
    free_cell(top_cell);
    return EXIT_FAILURE;
  }

  free_cell(bottom_cell);
  free_cell(top_cell);
  return EXIT_SUCCESS;
}

int test_get_top_cell_oob_neighbour() {
  cell bottom_cell = alloc_cell();
  cell top_cell = make_out_of_bounds_cell();
  set_top_cell(bottom_cell, top_cell);

  if (!is_out_of_bounds_cell(get_top_cell(bottom_cell))) {
    FPRINTF(stderr,
            "Error: test_get_top_cell_oob_neighbour, the top neighbour is "
            "not out of bounds.\n");
    free_cell(bottom_cell);
    return EXIT_FAILURE;
  }

  free_cell(bottom_cell);
  return EXIT_SUCCESS;
}

int test_get_top_cell_null_cell() {
  cell fake_cell = make_out_of_bounds_cell();
  if (get_top_cell(fake_cell) != NULL) {
    FPRINTF(stderr,
            "Error: test_get_top_cell_null_cell, function returned something "
            "other than NULL on an invalid cell");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int test_get_bottom_cell_valid() {
  cell bottom_cell = alloc_cell();
  cell top_cell = alloc_cell();
  set_bottom_cell(top_cell, bottom_cell);

  if (get_bottom_cell(top_cell) != bottom_cell) {
    FPRINTF(stderr,
            "Error: test_get_bottom_cell_valid, the bottom neighbour is "
            "different from bottom_cell.\n");
    free_cell(bottom_cell);
    free_cell(top_cell);
    return EXIT_FAILURE;
  }

  free_cell(bottom_cell);
  free_cell(top_cell);
  return EXIT_SUCCESS;
}

int test_get_bottom_cell_oob_neighbour() {
  cell bottom_cell = make_out_of_bounds_cell();
  cell top_cell = alloc_cell();
  set_bottom_cell(top_cell, bottom_cell);

  if (!is_out_of_bounds_cell(get_bottom_cell(top_cell))) {
    FPRINTF(
        stderr,
        "Error: test_get_bottom_cell_oob_neighbour, the bottom neighbour is "
        "not out of bounds.\n");
    free_cell(top_cell);
    return EXIT_FAILURE;
  }

  free_cell(top_cell);
  return EXIT_SUCCESS;
}

int test_get_bottom_cell_null_cell() {
  cell fake_cell = make_out_of_bounds_cell();
  if (get_bottom_cell(fake_cell) != NULL) {
    FPRINTF(
        stderr,
        "Error: test_get_bottom_cell_null_cell, function returned something "
        "other than NULL on an invalid cell");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int test_get_piece_cell_valid() {
  cell new_cell = alloc_cell();
  if (get_piece_cell(new_cell) != EMPTY) {
    FPRINTF(stderr,
            "Error: test_get_piece_cell_valid, new cell has piece %d instead "
            "of default %d.\n",
            get_piece_cell(new_cell), EMPTY);
    free_cell(new_cell);
    return EXIT_FAILURE;
  }
  free_cell(new_cell);
  return EXIT_SUCCESS;
}

int test_get_piece_cell_null_cell() {
  cell fake_cell = make_out_of_bounds_cell();
  if (get_piece_cell(fake_cell) != (piece)-2) {
    FPRINTF(stderr,
            "Error: test_get_piece_cell_valid, function returns something "
            "other than EMPTY on an empty cell.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int test_set_piece_cell_valid() {
  cell new_cell = alloc_cell();
  for (piece new_piece = EMPTY; new_piece < NB_PIECE_TYPE; new_piece++) {
    set_piece_cell(new_cell, new_piece);
    if (get_piece_cell(new_cell) != new_piece) {
      FPRINTF(stderr,
              "Error: test_set_piece_cell_valid, current piece is %d instead "
              "of newly set piece %d.\n",
              get_piece_cell(new_cell), new_piece);
      free_cell(new_cell);
      return EXIT_FAILURE;
    }
  }
  free_cell(new_cell);
  return EXIT_SUCCESS;
}

int test_set_piece_cell_null_cell() {
  cell fake_cell = make_out_of_bounds_cell();
  for (piece new_piece = EMPTY; new_piece < NB_PIECE_TYPE; new_piece++) {
    set_piece_cell(fake_cell, new_piece);
    if (get_piece_cell(fake_cell) == new_piece) {
      FPRINTF(stderr,
              "Error: test_set_piece_cell_valid, current piece is %d instead "
              "of newly set piece %d.\n",
              get_piece_cell(fake_cell), new_piece);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

int test_link_lines_cell_valid() {
  uint16_t width = 42;
  cell bottom_line = create_cell_line(width);
  cell top_line = create_cell_line(width);
  if (!link_lines_cell(bottom_line, top_line)) {
    FPRINTF(stderr,
            "Error: test_link_lines_cell_valid, function returned false on a "
            "valid call.\n");
    destroy_cell_line(bottom_line);
    destroy_cell_line(top_line);
    return EXIT_FAILURE;
  }

  cell current_bottom_cell = bottom_line;
  cell current_top_cell = top_line;
  for (uint16_t i = 0; i < width; i++) {
    if (get_top_cell(current_bottom_cell) != current_top_cell) {
      FPRINTF(stderr,
              "Error: test_link_lines_cell_valid, the top neightbour of "
              "current_bottom_cell is not current_top_cell.\n");
      destroy_cell_line(bottom_line);
      destroy_cell_line(top_line);
      return EXIT_FAILURE;
    } else if (get_bottom_cell(current_top_cell) != current_bottom_cell) {
      FPRINTF(stderr,
              "Error: test_link_lines_cell_valid, the bottom neightbour of "
              "current_top_cell is not current_bottom_cell.\n");
      destroy_cell_line(bottom_line);
      destroy_cell_line(top_line);
      return EXIT_FAILURE;
    }
  }

  destroy_cell_line(bottom_line);
  destroy_cell_line(top_line);
  return EXIT_SUCCESS;
}

int test_link_lines_cell_different_width() {
  uint16_t bottom_width = 42;
  cell bottom_line = create_cell_line(bottom_width);

  uint16_t top_width = 24;
  cell top_line = create_cell_line(top_width);

  if (link_lines_cell(bottom_line, top_line)) {
    FPRINTF(stderr,
            "Error: test_link_lines_cell_different_width, function returned "
            "true on an "
            "invalid call.\n");
    destroy_cell_line(bottom_line);
    destroy_cell_line(top_line);
    return EXIT_FAILURE;
  }
  destroy_cell_line(bottom_line);
  destroy_cell_line(top_line);
  return EXIT_SUCCESS;
}

int test_link_lines_cell_null_top() {
  uint16_t bottom_width = 42;
  cell bottom_line = create_cell_line(bottom_width);

  cell top_line = make_out_of_bounds_cell();

  if (link_lines_cell(bottom_line, top_line)) {
    FPRINTF(stderr,
            "Error: test_link_lines_cell_different_width, function returned "
            "true on an "
            "invalid call.\n");
    destroy_cell_line(bottom_line);
    return EXIT_FAILURE;
  }
  destroy_cell_line(bottom_line);
  return EXIT_SUCCESS;
}

int test_link_lines_cell_null_bottom() {
  cell bottom_line = make_out_of_bounds_cell();

  uint16_t top_width = 24;
  cell top_line = create_cell_line(top_width);

  if (link_lines_cell(bottom_line, top_line)) {
    FPRINTF(stderr,
            "Error: test_link_lines_cell_different_width, function returned "
            "true on an "
            "invalid call.\n");
    destroy_cell_line(top_line);
    return EXIT_FAILURE;
  }
  destroy_cell_line(top_line);
  return EXIT_SUCCESS;
}

int test_link_lines_cell_both_null() {
  cell bottom_line = make_out_of_bounds_cell();
  cell top_line = make_out_of_bounds_cell();

  if (link_lines_cell(bottom_line, top_line)) {
    FPRINTF(stderr,
            "Error: test_link_lines_cell_different_width, function returned "
            "true on an "
            "invalid call.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void usage(char* program_name) {
  FPRINTF(stderr, "Usage: %s <testname>\n", program_name);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
  if (argc == 1) usage(argv[0]);

  PRINTF("=> RUN TEST \"%s\"\n", argv[1]);

  int status;
  if (strcmp("alloc_cell", argv[1]) == 0)
    status = test_alloc_cell();
  else if (strcmp("free_cell_valid", argv[1]) == 0)
    status = test_free_cell_valid();
  else if (strcmp("free_cell_null_cell", argv[1]) == 0)
    status = test_free_cell_null_cell();
  else if (strcmp("get_right_cell_valid", argv[1]) == 0)
    status = test_get_right_cell_valid();
  else if (strcmp("get_right_cell_oob_neighbour", argv[1]) == 0)
    status = test_get_right_cell_oob_neighbour();
  else if (strcmp("get_right_cell_null_cell", argv[1]) == 0)
    status = test_get_right_cell_null_cell();
  else if (strcmp("get_left_cell_valid", argv[1]) == 0)
    status = test_get_left_cell_valid();
  else if (strcmp("get_left_cell_oob_neighbour", argv[1]) == 0)
    status = test_get_left_cell_oob_neighbour();
  else if (strcmp("get_left_cell_null_cell", argv[1]) == 0)
    status = test_get_left_cell_null_cell();
  else if (strcmp("get_top_cell_valid", argv[1]) == 0)
    status = test_get_top_cell_valid();
  else if (strcmp("get_top_cell_oob_neighbour", argv[1]) == 0)
    status = test_get_top_cell_oob_neighbour();
  else if (strcmp("get_top_cell_null_cell", argv[1]) == 0)
    status = test_get_top_cell_null_cell();
  else if (strcmp("get_bottom_cell_valid", argv[1]) == 0)
    status = test_get_bottom_cell_valid();
  else if (strcmp("get_bottom_cell_oob_neighbour", argv[1]) == 0)
    status = test_get_bottom_cell_oob_neighbour();
  else if (strcmp("get_bottom_cell_null_cell", argv[1]) == 0)
    status = test_get_bottom_cell_null_cell();
  else if (strcmp("get_piece_cell_valid", argv[1]) == 0)
    status = test_get_piece_cell_valid();
  else if (strcmp("get_piece_cell_null_cell", argv[1]) == 0)
    status = test_get_piece_cell_null_cell();
  else if (strcmp("set_piece_cell_valid", argv[1]) == 0)
    status = test_set_piece_cell_valid();
  else if (strcmp("set_piece_cell_null_cell", argv[1]) == 0)
    status = test_set_piece_cell_null_cell();
  else if (strcmp("link_lines_cell_valid", argv[1]) == 0)
    status = test_link_lines_cell_valid();
  else if (strcmp("link_lines_cell_different_width", argv[1]) == 0)
    status = test_link_lines_cell_different_width();
  else if (strcmp("link_lines_cell_null_top", argv[1]) == 0)
    status = test_link_lines_cell_null_top();
  else if (strcmp("link_lines_cell_null_bottom", argv[1]) == 0)
    status = test_link_lines_cell_null_bottom();
  else if (strcmp("link_lines_cell_both_null", argv[1]) == 0)
    status = test_link_lines_cell_both_null();
  else {
    FPRINTF(stderr, "Error: test %s not found!\n", argv[1]);
    return EXIT_FAILURE;
  }

  if (status != EXIT_SUCCESS)
    PRINTF("FAILURE (status %d)\n", status);
  else
    PRINTF("SUCCESS (status %d)\n", status);
  return status;
}
