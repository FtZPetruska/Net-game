#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool_array.h"
#include "cross_io.h"

#define NB_LINES 10
#define LINE_LENGTH 10

static int test_alloc_double_bool_array() {
  bool** double_array = alloc_double_bool_array(NB_LINES, LINE_LENGTH);

  if (!double_array) {
    FPRINTF(
        stderr,
        "Error: test_alloc_bool_array, the returned double_array is NULL.\n");
    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < NB_LINES; i++) {
    if (!double_array[i]) {
      FPRINTF(stderr, "Error: test_alloc_bool_array, line %zu is NULL.\n", i);
      return EXIT_FAILURE;
    }
    for (size_t j = 0; j < LINE_LENGTH; j++) {
      if (double_array[i][j]) {
        FPRINTF(stderr,
                "Error: test_alloc_bool_array, cell [%zu][%zu] has been "
                "initialized to true. (Expected false)\n",
                i, j);
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}

static int test_free_double_bool_array_valid() {
  bool** double_array = alloc_double_bool_array(NB_LINES, LINE_LENGTH);
  free_double_bool_array(double_array, NB_LINES);
  return EXIT_SUCCESS;
}

static int test_free_double_bool_array_null_array() {
  free_double_bool_array(NULL, NB_LINES);
  return EXIT_SUCCESS;
}

static int test_free_double_bool_array_null_line() {
  bool** fake_array = (bool**)calloc(NB_LINES, sizeof(bool*));
  free_double_bool_array(fake_array, NB_LINES);
  return EXIT_SUCCESS;
}

static int test_check_double_bool_array_true() {
  bool** true_array = alloc_double_bool_array(NB_LINES, LINE_LENGTH);
  for (size_t i = 0; i < NB_LINES; i++) {
    for (size_t j = 0; j < LINE_LENGTH; j++) {
      true_array[i][j] = true;
    }
  }
  if (!check_double_bool_array(true_array, NB_LINES, LINE_LENGTH)) {
    FPRINTF(stderr,
            "Error: test_check_double_bool_array_true, function returned false "
            "on a valid array. (Expected true)\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static int test_check_double_bool_array_false() {
  bool** true_array = alloc_double_bool_array(NB_LINES, LINE_LENGTH);
  if (check_double_bool_array(true_array, NB_LINES, LINE_LENGTH)) {
    FPRINTF(stderr,
            "Error: test_check_double_bool_array_true, function returned true "
            "on an invalid array. (Expected false)\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static int test_check_double_bool_array_null_array() {
  if (check_double_bool_array(NULL, NB_LINES, LINE_LENGTH)) {
    FPRINTF(stderr,
            "Error: test_check_double_bool_array_null_array, function returned "
            "true on a NULL array. (Expected false)\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static int test_check_double_bool_array_null_line() {
  bool* fake_array[] = {NULL};
  if (check_double_bool_array(fake_array, NB_LINES, LINE_LENGTH)) {
    FPRINTF(stderr,
            "Error: test_check_double_bool_array_null_line, function returned "
            "true on an array with a NULL line. (Expected false)\n");
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
  if (strcmp("alloc_double_bool_array", argv[1]) == 0)
    status = test_alloc_double_bool_array();
  else if (strcmp("free_double_bool_array_valid", argv[1]) == 0)
    status = test_free_double_bool_array_valid();
  else if (strcmp("free_double_bool_array_null_array", argv[1]) == 0)
    status = test_free_double_bool_array_null_array();
  else if (strcmp("free_double_bool_array_null_line", argv[1]) == 0)
    status = test_free_double_bool_array_null_line();
  else if (strcmp("check_double_bool_array_true", argv[1]) == 0)
    status = test_check_double_bool_array_true();
  else if (strcmp("check_double_bool_array_false", argv[1]) == 0)
    status = test_check_double_bool_array_false();
  else if (strcmp("check_double_bool_array_null_array", argv[1]) == 0)
    status = test_check_double_bool_array_null_array();
  else if (strcmp("check_double_bool_array_null_line", argv[1]) == 0)
    status = test_check_double_bool_array_null_line();
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
