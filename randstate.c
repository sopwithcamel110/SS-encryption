#include "randstate.h"

gmp_randstate_t state;

/**
 * Initializes the randstate variable state using seed
 */
void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}
/**
 * Clears the randstate variable state
 */
void randstate_clear() {
    gmp_randclear(state);
}
