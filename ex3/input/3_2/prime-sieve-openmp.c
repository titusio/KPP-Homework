// serial prime sieve implementation
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <pthread.h>
#define NUM_THREADS 5

#if !defined(_OPENMP) && !defined(SERIAL) && !defined(PTHREAD)
#define SERIAL
#endif

#if defined(_OPENMP) || defined(SERIAL)
int prime_sieve(long max_prime, char **primes_array){
  int count = 0;

  *primes_array = malloc(max_prime * sizeof(char));
  char *primes = *primes_array;
  if (primes == NULL) {
    perror("malloc");
    return -1;
  }

  for (long i = 0; i < max_prime; i++) {
    primes[i] = 1;
  }

  primes[0] = 0;
  primes[1] = 0;

  int sqrt_max_prime = sqrt(max_prime);
  #pragma omp parallel for 
  for (long i = 2; i <= sqrt_max_prime; i++) {
    if (primes[i]) {
      for (long j = i * i; j < max_prime; j += i) {
        primes[j] = 0;
      }
    }
  }

  #pragma omp parallel for 
  for (long i = 2; i < max_prime; i++) {
    if (primes[i]) {
      count++;
    }
  }
  return count;
}
#endif

void print_primes(int count,long max_prime, char* primes){
  printf("Found %d Primes: \n", count);
  for (long n = 2; n< max_prime; n++){
    if (primes[n]){
      printf("%ld ", n);
    }
  }
}

int main(int argc, char** argv){
  long max_prime;
  char *endptr;


  if (argc < 2) {
    printf("Usage: %s <max prime>\n", argv[0]);
    return 1;
  }
  errno = 0;
  max_prime = strtol(argv[1], &endptr, 10);
  if (errno != 0 || *endptr != '\0') {
    printf("Invalid number: %s\n", argv[1]);
    return 1;
  }

  char* primes;
  int count = prime_sieve(max_prime, &primes);
  if (count <0)
    exit(EXIT_FAILURE);

  print_primes(count, max_prime, primes);

  free(primes);
}

