#include <stdint.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
  uint32_t id;
  float_t m;
  float_t rx, ry, rz;
  float_t vx, vy, vz;
} particle_t;

#define ELEMENT_COUNT 10000000
#define ITERATIONS 1000

void initElements(particle_t *particles) {
  for (size_t i = 0; i < ELEMENT_COUNT; i++) {
    particles[i].id = i;
    particles[i].m = rand();
    particles[i].rx = rand();
    particles[i].ry = rand();
    particles[i].rz = rand();
    particles[i].vx = rand();
    particles[i].vy = rand();
    particles[i].vz = rand();
  }
}

void calculate(particle_t *particle, float_t dt) {
  particle->rx += particle->vx * dt;
  particle->ry += particle->vy * dt;
  particle->rz += particle->vz * dt;
}

int main() {
  particle_t *particles = calloc(ELEMENT_COUNT, sizeof(*particles));
  initElements(particles);

  for (size_t i = 0; i < ITERATIONS; i += 1) {
    for (size_t j = 0; j < ELEMENT_COUNT; j += 1) {
      calculate(particles + j, 3.14f);
    }
  }

  return 0;
}
