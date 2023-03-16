#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ss.h"

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pbfile = fopen("ss.pub", "r");
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
            pbfile = fopen(optarg, "r");
            if (!pbfile) {
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
        printf("   Encrypts data using SS encryption.\n");
        printf("   Encrypted data is decrypted by the decrypt program.\n\n");
        printf("USAGE\n");
        printf("./encrypt [OPTIONS]\n\n");
        printf("OPTIONS\n");
        printf("   -h		   Display program help and usage.\n");
        printf("   -v		   Display verbose program output.\n");
        printf("   -i infile	   Input file of data to encrypt (default: stdin).\n");
        printf("   -o outfile	   Output file for encrypted data (default: stdout).\n");
        printf("   -n pbfile	   Public key file (default: ss.pub).\n");
        return 0;
    }
    // Get public key
    char username[50];
    mpz_t pubKey;
    mpz_init(pubKey);
    ss_read_pub(pubKey, username, pbfile);
    if (verbose) {
        printf("user: %s", username);
        gmp_printf("n (%d bits): %Zd", mpz_sizeinbase(pubKey, 2), pubKey);
    }
    // Encrypt
    ss_encrypt_file(infile, outfile, pubKey);

    mpz_clear(pubKey);
    fclose(outfile);
    fclose(infile);
    fclose(pbfile);
    return 0;
}
