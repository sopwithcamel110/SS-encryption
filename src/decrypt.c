#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ss.h"

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvfile = fopen("ss.priv", "r");
    int verbose = 0, showHelp = 0;
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "r");
            if (!infile) {
                printf("Error occurred opening: %s", optarg);
                return 1;
            }
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            if (!outfile) {
                printf("Error occurred opening: %s", optarg);
                return 1;
            }
            break;
        case 'n':
            pvfile = fopen(optarg, "r");
            if (!pvfile) {
                printf("Error occurred opening: %s", optarg);
                return 1;
            }
            break;
        case 'v': verbose = 1; break;
        case 'h': showHelp = 1; break;
        }
    }
    if (showHelp) {
        printf("SYNOPSIS\n");
        printf("   Decrypts data using SS encryption.\n");
        printf("   Data is encrypted by the encrypt program.\n\n");
        printf("USAGE\n");
        printf("./decrypt [OPTIONS]\n\n");
        printf("OPTIONS\n");
        printf("   -h		   Display program help and usage.\n");
        printf("   -v		   Display verbose program output.\n");
        printf("   -i infile	   Input file of data to encrypt (default: stdin).\n");
        printf("   -o outfile	   Output file for encrypted data (default: stdout).\n");
        printf("   -n pvfile	   Private key file (default: ss.priv).\n");
        return 0;
    }
    // Get private key
    mpz_t pq, d;
    mpz_init(pq);
    mpz_init(d);
    ss_read_priv(pq, d, pvfile);
    if (verbose) {
        gmp_printf("pq (%d bits): %Zd", mpz_sizeinbase(pq, 2), pq);
        gmp_printf("d (%d bits): %Zd", mpz_sizeinbase(d, 2), d);
    }
    // Decrypt
    ss_decrypt_file(infile, outfile, d, pq);

    mpz_clear(pq);
    mpz_clear(d);
    fclose(outfile);
    fclose(infile);
    fclose(pvfile);
    return 0;
}
