#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define ELEMENT_COUNT 10000000
#define ITERATIONS 1000

void randomize(float_t *array, size_t count) {
  for (size_t i = 0; i < count; i++) {
    array[i] = rand();
  }
}

int main() {
  int32_t *ids = calloc(ELEMENT_COUNT, sizeof(*ids));
  float_t *masses = calloc(ELEMENT_COUNT, sizeof(*masses));
  randomize(masses, ELEMENT_COUNT);

  float_t *rxs = calloc(ELEMENT_COUNT, sizeof(*rxs));
  float_t *rys = calloc(ELEMENT_COUNT, sizeof(*rys));
  float_t *rzs = calloc(ELEMENT_COUNT, sizeof(*rzs));
  randomize(rxs, ELEMENT_COUNT);
  randomize(rys, ELEMENT_COUNT);
  randomize(rzs, ELEMENT_COUNT);

  float_t *vxs = calloc(ELEMENT_COUNT, sizeof(*vxs));
  float_t *vys = calloc(ELEMENT_COUNT, sizeof(*vys));
  float_t *vzs = calloc(ELEMENT_COUNT, sizeof(*vzs));
  randomize(vxs, ELEMENT_COUNT);
  randomize(vys, ELEMENT_COUNT);
  randomize(vzs, ELEMENT_COUNT);

  for (size_t iteration = 0; iteration < ITERATIONS; iteration += 1) {
    for (size_t element = 0; element < ELEMENT_COUNT; element += 1) {
      rxs[element] += vxs[element] * 3.14f;
      rys[element] += vys[element] * 3.14f;
      rzs[element] += vzs[element] * 3.14f;
    }
  }

  return 0;
}
