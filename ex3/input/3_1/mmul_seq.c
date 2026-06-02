/*
 * Paralleles Rechnen, Uebung 1
 * Sequentielle Matrix-Multiplikation nach Schulmethode
 *
 * Build default: B in normal layout
 * Build with -DUSE_TRANSPOSED_B: B is stored transposed for cache-friendly
 * access
 *
 * vim: expandtab shiftwidth=4 tabstop=4
 */

#include <bits/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum { DEFAULT_MATRIX_SIZE = 4096 };

typedef double matrix_elem_t;

typedef struct {
  int rows, cols;
  matrix_elem_t *data;
} matrix_t;

static inline double time_diff_sec(struct timespec timer1,
                                   struct timespec timer2) {
  return ((timer2.tv_sec * 1.0E+9 + timer2.tv_nsec) -
          (timer1.tv_sec * 1.0E+9 + timer1.tv_nsec)) /
         1.0E+9;
}

static int init_matrix(matrix_t *m, int size) {
  m->rows = size;
  m->cols = size;
  m->data = malloc((size_t)m->rows * (size_t)m->cols * sizeof(matrix_elem_t));
  if (m->data == NULL) {
    perror("malloc failed");
    return -1;
  }

  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      m->data[(i * m->cols) + j] = (matrix_elem_t)i / ((matrix_elem_t)j + 1.0);
    }
  }
  return 0;
}

int write_matrix(const char *filepath, matrix_t *matrix) {
  int i, j;
  FILE *fp;

  if ((fp = fopen(filepath, "w")) == NULL) {
    perror("Can't open file!");
    exit(EXIT_FAILURE);
  }

  fprintf(fp, "%d\n", matrix->rows);
  fprintf(fp, "%d\n", matrix->cols);

  for (i = 0; i < matrix->rows; i++) {
    for (j = 0; j < matrix->cols; j++) {
      fprintf(fp, "%.8lf\t", matrix->data[(i * matrix->cols) + j]);
    }
    fprintf(fp, "\n");
  }

  fclose(fp);
  return 0;
}

static bool matrix_mult(const matrix_t *a, const matrix_t *b, matrix_t *r) {
  if (a->cols != b->rows) {
    return false;
  }

  r->rows = a->rows;
  r->cols = b->cols;
  r->data = calloc((size_t)r->rows * (size_t)r->cols, sizeof(matrix_elem_t));
  if (r->data == NULL) {
    return false;
  }

  // first run took: 701.976925
  for (int i = 0; i < r->rows; i++) {
    for (int j = 0; j < r->cols; j++) {
      matrix_elem_t y = 0;
      for (int k = 0; k < a->cols; ++k) {
        y += a->data[i * a->cols + k] * b->data[k * b->cols + j];
      }
      r->data[i * r->cols + j] = y;
    }
  }

  return true;
}

int main(int argc, char *argv[]) {
  matrix_t a = {0, 0, NULL};
  matrix_t b = {0, 0, NULL};
  matrix_t r = {0, 0, NULL};

  int matrix_size = DEFAULT_MATRIX_SIZE;
  if (argc == 2) {
    char *endptr = NULL;
    unsigned long parsed = strtoul(argv[1], &endptr, 10);
    if (endptr == argv[1] || *endptr != '\0' || parsed == 0UL ||
        parsed > INT32_MAX) {
      fprintf(stderr, "Usage: %s [matrix_size]\n", argv[0]);
      return EXIT_FAILURE;
    }
    matrix_size = (int)parsed;
  } else if (argc > 2) {
    fprintf(stderr, "Usage: %s [matrix_size]\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (init_matrix(&a, matrix_size) != 0) {
    fprintf(stderr, "Could not initialize A matrix\n");
    return EXIT_FAILURE;
  }

  if (init_matrix(&b, matrix_size) != 0) {
    fprintf(stderr, "Could not initialize B matrix\n");
    free(a.data);
    return EXIT_FAILURE;
  }

  struct timespec t1, t2;
  clock_gettime(CLOCK_MONOTONIC, &t1);

  if (!matrix_mult(&a, &b, &r)) {
    fprintf(stderr, "Could not multiply matrices (regular path).\n");
    free(a.data);
    free(b.data);
    return EXIT_FAILURE;
  }

  clock_gettime(CLOCK_MONOTONIC, &t2);

  printf("time %lf \n", time_diff_sec(t1, t2));

  write_matrix("result.txt", &r);

  free(a.data);
  free(b.data);
  free(r.data);

  return EXIT_SUCCESS;
}
