#include "bool_array.h"

#include "cross_io.h"

bool** alloc_double_bool_array(size_t nb_lines, size_t line_length) {
  bool** new_array = (bool**)malloc(sizeof(bool*) * nb_lines);
  if (!new_array) {
    FPRINTF(stderr,
            "Error: not enough memory to allocate double boolean array.\n");
    return NULL;
  }

  for (size_t i = 0; i < nb_lines; i++) {
    new_array[i] = (bool*)malloc(sizeof(bool) * line_length);
    if (!new_array[i]) {
      FPRINTF(stderr,
              "Error: not enough memory to allocate a line of a double boolean "
              "array.\n");
      free_double_bool_array(new_array, i);
      return NULL;
    }
    for (size_t j = 0; j < line_length; j++) {
      new_array[i][j] = false;
    }
  }
  return new_array;
}

void free_double_bool_array(bool** double_array, size_t nb_lines) {
  if (!double_array) {
    FPRINTF(stderr,
            "Error: free_double_bool_array expected a double_array but got "
            "NULL pointer.\n");
    return;
  }
  for (size_t i = 0; i < nb_lines; i++) {
    if (double_array[i]) {
      free(double_array[i]);
    }
  }
  free(double_array);
}

bool check_double_bool_array(bool** double_array, size_t nb_lines,
                             size_t line_length) {
  if (!double_array) {
    FPRINTF(stderr,
            "Error: check_double_bool_array expected double_array but got NULL "
            "pointer.\n");
    return false;
  }
  for (size_t i = 0; i < nb_lines; i++) {
    if (!double_array[i]) {
      FPRINTF(stderr, "Error: check_double_bool_array, line %zu is NULL.\n", i);
      return false;
    }
    for (size_t j = 0; j < line_length; j++) {
      if (!double_array[i][j]) {
        return false;
      }
    }
  }
  return true;
}
