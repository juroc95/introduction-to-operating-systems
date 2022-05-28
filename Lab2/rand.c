#include "rand.h"
#include "types.h"
#include "user.h"

static unsigned long next = 1;

/* RAND_MAX assumed to be 1 << 31 */
unsigned int rand(void) {
    next = next * 1103515245 + 12345;
    return((unsigned)(next/65536) % RAND_MAX);
}

void srand(unsigned int seed) {
    next = seed;
}
