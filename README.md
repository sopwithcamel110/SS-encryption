# Public Key Cryptography

## Description
A showcase of SS Cryptography.

## How to Run
1. Navigate to the bin folder, or run "make" to create the binaries.
2. Run ./keygen [OPTIONS] to create the public and private keys. Then, encrypt messages with ./encrypt [OPTIONS] and decrypt them with ./decrypt [OPTIONS].

## Command Line Options: Keygen
-b : specifies the minimum bits needed for the public modulus n.   
-i : specifies the number of Miller-Rabin iterations for testing primes (default: 50).   
-n pbfile : specifies the public key file (default: ss.pub).   
-d pvfile : specifies the private key file (default: ss.priv).   
-s : specifies the random seed for the random state initialization (default: the seconds since the
UNIX epoch, given by time(NULL)).   
-v : enables verbose output.   
-h : displays program synopsis and usage.   

## Command Line Options: Encrypt
-i : specifies the input file to encrypt (default: stdin).   
-o : specifies the output file to encrypt (default: stdout).   
-n : specifies the file containing the public key (default: ss.pub).   
-v : enables verbose output.   
-h : displays program synopsis and usage.   

## Command Line Options: Decrypt
-i : specifies the input file to decrypt (default: stdin).   
-o : specifies the output file to decrypt (default: stdout).   
-n : specifies the file containing the private key (default: ss.priv).   
-v : enables verbose output.   
-h : displays program synopsis and usage.   

## Example
To showcase all binaries in a simple test
```sh
./keygen -b 300
echo "Lorem ipsum hello world" > message.txt
./encrypt -i message.txt -o out.txt
./decrypt -i out.txt
```
