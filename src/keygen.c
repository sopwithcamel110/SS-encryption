#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include "randstate.h"
#include "ss.h"

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv) {
    int verbose = 0, showHelp = 0, minBits = 256, iters = 50;
    FILE *pbfile = fopen("ss.pub", "w");
    FILE *pvfile = fopen("ss.priv", "w");
    if (!pbfile || !pvfile) {
        printf("Error opening ss.pub or ss.priv");
        return 0;
    }
    int seed = time(NULL);
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': minBits = atoi(optarg); break;
        case 'i': iters = atoi(optarg); break;
        case 'n':
            pbfile = fopen(optarg, "w");
            if (!pbfile) {
                printf("Error opening %s.\n", optarg);
            }
            break;
        case 'd':
            pvfile = fopen(optarg, "w");
            if (!pvfile) {
                printf("Error opening %s.\n", optarg);
                return 0;
            }
            break;
        case 's': seed = atoi(optarg); break;
        case 'v': verbose = 1; break;
        case 'h': showHelp = 1;
        }
    }
    if (showHelp) {
        printf("SYNOPSIS\n");
        printf("   Generates an SS public/private key pair.\n\n");
        printf("USAGE\n");
        printf("./keygen [OPTIONS]\n\n");
        printf("OPTIONS\n");
        printf("   -h		   Display program help and usage.\n");
        printf("   -v		   Display verbose program output.\n");
        printf("   -b bits	   Minimum bits needed for public key n (default: 256).\n");
        printf("   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n");
        printf("   -n pbfile	   Public key file (default: ss.pub).\n");
        printf("   -d pvfile	   Private key file (default: ss.priv).\n");
        printf("   -s seed	   Random seed for testing.\n");
        return 0;
    }
    // Set file permissions
    int desc = fileno(pvfile);
    fchmod(desc, 0600);
    // Make public key
    randstate_init(seed);
    mpz_t p, q, n;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    ss_make_pub(p, q, n, minBits, iters);
    // Make private key
    mpz_t d, pq;
    mpz_init(d);
    mpz_init(pq);
    ss_make_priv(d, pq, p, q);
    // Write public and private keys to their respective files
    char *username = getenv("USER");
    ss_write_pub(n, username, pbfile);
    ss_write_priv(pq, d, pvfile);
    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("pq (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);

    mpz_clear(pq);
    mpz_clear(d);
    return 0;
}
