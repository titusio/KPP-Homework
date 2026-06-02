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

typedef struct{
  char* primes_array;
  long start;
  long end;
  long max_prime;
} prime_arg_t;

void* thread_prime_sieve(void *arg){
  long start = ((prime_arg_t*) arg)->start;
  long end = ((prime_arg_t*) arg)->end;
  long max_prime = ((prime_arg_t*) arg)->max_prime;
  char* primes = ((prime_arg_t*) arg)->primes_array;

  for(long i=start; i< end; i++){
    if (primes[i]) {
      for (long j = i * i; j < max_prime; j += i) {
        primes[j] = 0;
      }
    }
  }

  return NULL;
}

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

  int ret=0;
  pthread_t threads[NUM_THREADS];
  prime_arg_t args[NUM_THREADS];

  for (int i=0; i< NUM_THREADS; i++){
    args[i].max_prime=max_prime;
    int range = (sqrt_max_prime-2)/NUM_THREADS;
    args[i].start = 2+range*i;
    args[i].end = 2+range*(i+1);
    args[i].primes_array = primes+(range*i);
    if (i== NUM_THREADS-1)
      args[i].end+= (sqrt_max_prime-2)%NUM_THREADS+1;

    ret=pthread_create(threads+i, NULL, thread_prime_sieve, (void*) &args[i]);
  }

  for (int i=0; i< NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
  }

  for (long i = 2; i < max_prime; i++) {
    if (primes[i]) {
      count++;
    }
  }
  return count;
}

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

