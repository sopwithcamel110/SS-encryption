#include <stdlib.h>
#include <stdio.h>
#include "ss.h"
#include "numtheory.h"
#include "randstate.h"

extern gmp_randstate_t state;

/**
 * Sets rop to the least common multiple of a and b
 */
void lcm(mpz_t rop, const mpz_t a, const mpz_t b) {
    // rop = |a*b|
    mpz_mul(rop, a, b);
    mpz_abs(rop, rop);
    // g = gcd(a,b)
    mpz_t g;
    mpz_init(g);
    gcd(g, a, b);
    // rop = rop/g
    mpz_tdiv_q(rop, rop, g);

    mpz_clear(g);
}
/**
 * Creates primes p and q, as well as n=p*p*q where n is nbits bits and primality is tested using iters iterations.
 */
void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    uint64_t N = (random() % (nbits / 5)) + (nbits / 5);
    // make p and q
    mpz_t pMinus1, qMinus1;
    mpz_init(pMinus1);
    mpz_init(qMinus1);
    do {
        make_prime(p, N, iters);
        make_prime(q, nbits - (N * 2) + 4, iters);
        mpz_sub_ui(pMinus1, p, 1);
        mpz_sub_ui(qMinus1, q, 1);
    } while (mpz_divisible_p(p, qMinus1) || mpz_divisible_p(q, pMinus1));
    // n = p*p*q
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);
    mpz_clear(pMinus1);
    mpz_clear(qMinus1);
}
/**
 * Creates private key d = n inverse mod lcm(p-1, q-1) and number pq = p*q using primes p and q.
 */
void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    // temp var
    mpz_t n, pCopy, qCopy;
    mpz_init(n);
    mpz_init(pCopy);
    mpz_init(qCopy);

    mpz_sub_ui(pCopy, p, 1);
    mpz_sub_ui(qCopy, q, 1);
    // d = lcm(p-1, q-1)
    lcm(d, pCopy, qCopy);
    // n = p*p*q
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);
    // d = n invmod d
    mod_inverse(d, n, d);
    // pq = p x q
    mpz_mul(pq, p, q);

    mpz_clear(pCopy);
    mpz_clear(qCopy);
    mpz_clear(n);
}
/**
 * Writes public key to pbfile using mpz n and string username
 */
void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%s\n", n, username);
}
/**
 * Writes private key to pvfile using mpz's pq and d
 */
void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d);
}
/**
 * Reads n and username from pbfile
 */
void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%s\n", n, username);
}
/**
 * Reads pq and d from pvfile
 */
void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", pq, d);
}
/**
 * Sets c = pow_mod(m.n.n)
 */
void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
}
/**
 * Encrypts file infile using public key n and outputs to outfile
 */
void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t temp;
    mpz_init(temp);
    mpz_sqrt(temp, n);
    int k = (mpz_sizeinbase(temp, 2) - 1) / 8;
    uint8_t *block = calloc(k, sizeof(uint8_t));
    block[0] = 0xFF;
    int c = fgetc(infile), i = 1;
    mpz_t buffer;
    mpz_init(buffer);
    while (c != EOF) {
        if (i >= k) {
            mpz_import(buffer, k, 1, sizeof(block[0]), 1, 0, block);
            ss_encrypt(buffer, buffer, n);
            gmp_fprintf(outfile, "%Zd\n", buffer);
            i = 1;
        }
        block[i] = c;
        i++;
        c = fgetc(infile);
    }
    if (i != 1) {
        mpz_import(buffer, i, 1, sizeof(block[0]), 1, 0, block);
        ss_encrypt(buffer, buffer, n);
        gmp_fprintf(outfile, "%Zd\n", buffer);
    }
    mpz_clear(temp);
    mpz_clear(buffer);
    free(block);
}
/**
 * Sets m = pow_mod(c,d,pq)
 */
void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
}
/**
 * Decrypts file infile using d and pq and outputs to outfile
 */
void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    int k = (mpz_sizeinbase(pq, 2) - 1);
    uint8_t *block = calloc(k, sizeof(uint8_t));
    mpz_t buffer;
    mpz_init(buffer);
    unsigned long numRead = 0;
    while (gmp_fscanf(infile, "%Zd\n", buffer) != EOF) {
        ss_decrypt(buffer, buffer, d, pq);
        mpz_export(block, &numRead, 1, sizeof(block[0]), 1, 0, buffer);

        for (unsigned long i = 1; i < numRead; i++) {
            fputc(block[i], outfile);
        }
    }
    mpz_clear(buffer);
    free(block);
}
