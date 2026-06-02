#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ERROR (double)1e-6

struct error_metric {
  double median_error;
  double max_error;
  double mean_error;
};

double *read_file(const char *filename, size_t *length) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
    return NULL;
  }

  uint32_t num_rows, num_cols;
  if (fscanf(file, "%u\n", &num_rows) != 1) {
    fprintf(stderr, "Failed to read number of rows\n");
    fclose(file);
    return NULL;
  }

  if (fscanf(file, "%u\n", &num_cols) != 1) {
    fprintf(stderr, "Failed to read number of columns\n");
    fclose(file);
    return NULL;
  }

  *length = num_rows * num_cols;

  double *data = malloc(*length * sizeof(double));

  if (!data) {
    perror("Failed to allocate memory for data");
    fclose(file);
    return NULL;
  }

  for (size_t i = 0; i < *length; i++) {
    if (fscanf(file, "%lf\t", &data[i]) != 1) {
      fprintf(stderr, "Failed to read data at index %zu\n", i);
      free(data);
      fclose(file);
      return NULL;
    }
  }

  fclose(file);

  return data;
}

struct error_metric rel_error(const double *data1, const double *data2,
                              size_t length) {
  double *errors = malloc(length * sizeof(double));
  double sum_error = 0.0;
  if (!errors) {
    perror("Failed to allocate memory for errors");
    struct error_metric em = {-1.0, -1.0, -1.0};
    return em;
  }

  for (size_t i = 0; i < length; i++) {
    errors[i] = fabs(data1[i] - data2[i]);
    sum_error += errors[i];
  }

  qsort(errors, length, sizeof(double),
        (int (*)(const void *, const void *))strcmp);

  double median_error;
  if (length % 2 == 0) {
    median_error = (errors[length / 2 - 1] + errors[length / 2]) / 2.0;
  } else {
    median_error = errors[length / 2];
  }

  struct error_metric em = {median_error, errors[length - 1],
                            sum_error / length};

  free(errors);

  return em;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <first_file>.txt <second_file>.txt\n", argv[0]);
    return EXIT_FAILURE;
  }

  size_t length1, length2;

  double *data1 = read_file(argv[1], &length1);

  if (data1 == NULL) {
    return EXIT_FAILURE;
  }

  double *data2 = read_file(argv[2], &length2);
  if (data2 == NULL) {
    free(data1);
    return EXIT_FAILURE;
  }

  if (length1 != length2) {
    fprintf(stderr, "Files have different lengths: %zu vs %zu\n", length1,
            length2);
    free(data1);
    free(data2);
    return EXIT_FAILURE;
  }

  struct error_metric em = rel_error(data1, data2, length1);

  fprintf(stdout, "Relative Error Metrics:\n");

  fprintf(stdout, "Median Error: %e %%\n", em.median_error * 100.0);
  fprintf(stdout, "Max Error: %e %%\n", em.max_error * 100.0);
  fprintf(stdout, "Mean Error: %e %%\n", em.mean_error * 100.0);

  if (em.max_error > MAX_ERROR) {
    fprintf(stderr, "Files differ significantly.\n");
  } else {
    printf("Files are similar within the acceptable error margin of %e.\n",
           MAX_ERROR);
  }

  free(data1);
  free(data2);

  return EXIT_SUCCESS;
}
