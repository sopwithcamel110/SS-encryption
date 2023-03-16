#include "numtheory.h"
#include "randstate.h"

extern gmp_randstate_t state;

/**
 * Set g to the greatest common divisor of a and b
 */
void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t aCopy, bCopy, t;
    mpz_init(aCopy);
    mpz_init(bCopy);
    mpz_init(t);

    mpz_set(aCopy, a);
    mpz_set(bCopy, b);
    while (mpz_sgn(bCopy)) {
        mpz_set(t, bCopy);
        mpz_mod(bCopy, aCopy, bCopy);
        mpz_set(aCopy, t);
    }
    mpz_set(g, aCopy);

    mpz_clear(aCopy);
    mpz_clear(bCopy);
    mpz_clear(t);
}
/**
 * Set o to the inverse of a mod n
 */
void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r, rPrime, t, tPrime, q, temp;
    mpz_init(r);
    mpz_init(rPrime);
    mpz_init(t);
    mpz_init(tPrime);
    mpz_init(q);
    mpz_init(temp);

    mpz_set(r, n);
    mpz_set(rPrime, a);
    mpz_set_si(t, 0);
    mpz_set_si(tPrime, 1);
    while (mpz_sgn(rPrime)) {
        mpz_fdiv_q(q, r, rPrime);

        mpz_set(temp, r);
        mpz_set(r, rPrime);
        mpz_mul(rPrime, q, rPrime);
        mpz_sub(rPrime, temp, rPrime);

        mpz_set(temp, t);
        mpz_set(t, tPrime);
        mpz_mul(tPrime, q, tPrime);
        mpz_sub(tPrime, temp, tPrime);
    }
    if (mpz_cmp_si(r, 1) > 0) {
        mpz_set_ui(t, 0); // No inverse
    }
    if (mpz_cmp_si(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(o, t);

    mpz_clear(r);
    mpz_clear(rPrime);
    mpz_clear(t);
    mpz_clear(tPrime);
    mpz_clear(q);
    mpz_clear(temp);
}
/**
 * Sets o to a raised to the d power modulo n
 */
void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t aCopy, dCopy;
    mpz_init(aCopy);
    mpz_init(dCopy);
    mpz_set(aCopy, a);
    mpz_set(dCopy, d);

    mpz_set_ui(o, 1);
    while (mpz_cmp_ui(dCopy, 0) > 0) {
        if (mpz_odd_p(dCopy)) {
            mpz_mul(o, o, aCopy);
            mpz_mod(o, o, n);
        }
        mpz_mul(aCopy, aCopy, aCopy);
        mpz_mod(aCopy, aCopy, n);
        mpz_fdiv_q_ui(dCopy, dCopy, 2);
    }

    mpz_clear(aCopy);
    mpz_clear(dCopy);
}
/**
 * Returns whether n is prime using iters iterations of the Miller-Rabin probabilistic primality formula.
 */
bool is_prime(const mpz_t n, uint64_t iters) {
    if (mpz_cmp_si(n, 2) < 0) {
        return false;
    }
    if (mpz_even_p(n) && mpz_cmp_si(n, 2) != 0) {
        return false;
    }
    int s = -1;
    mpz_t r, curr, temp, mod, nVal;
    mpz_init(r);
    mpz_init(curr);
    mpz_init(temp);
    mpz_init(mod);
    mpz_init(nVal);
    mpz_set_ui(curr, 2);
    int count = 0;

    mpz_sub_ui(nVal, n, 1);
    // nVal = n-1
    while (mpz_cmp(curr, nVal) <= 0) {
        count++;
        mpz_mod(mod, nVal, curr);
        mpz_fdiv_q(temp, nVal, curr);
        if (mpz_sgn(mod) == 0 && mpz_odd_p(temp)) {
            s = count;
            mpz_set(r, temp);
        }
        mpz_mul_ui(curr, curr, 2);
    }
    mpz_clear(mod);
    mpz_clear(curr);
    mpz_t a, y;
    mpz_init(a);
    mpz_init(y);
    mpz_set_ui(temp, 2);
    // temp = 2
    for (uint64_t i = 0; i < iters; i++) {
        mpz_sub_ui(nVal, n, 4);
        // nVal = n-4
        mpz_urandomm(a, state, nVal);
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);

        mpz_sub_ui(nVal, n, 1);
        // nVal = n-1
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, nVal) != 0) {
            int j = 1;
            while (j <= s - 1 && mpz_cmp(y, nVal) != 0) {
                pow_mod(y, y, temp, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clear(r);
                    mpz_clear(temp);
                    mpz_clear(nVal);
                    mpz_clear(a);
                    mpz_clear(y);
                    return false;
                }
                j++;
            }
            if (mpz_cmp(y, nVal) != 0) {
                mpz_clear(r);
                mpz_clear(temp);
                mpz_clear(nVal);
                mpz_clear(a);
                mpz_clear(y);
                return false;
            }
        }
    }
    mpz_clear(r);
    mpz_clear(temp);
    mpz_clear(nVal);
    mpz_clear(a);
    mpz_clear(y);
    return true;
}
/**
 * Sets p to a prime of bits bits, checking the primality using iters iterations of is_prime()
 */
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    do {
        mpz_urandomb(p, state, bits);
        mpz_setbit(p, bits - 1);
        mpz_setbit(p, 0);
    } while (!is_prime(p, iters));
}
