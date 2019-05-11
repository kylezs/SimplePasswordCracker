/**
* Password cracker - project 2 Comp30023
* Author: Kyle Zsembery
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include "sha256.h"

#define PW4_FILE_LOC "./pwd4sha256"

#define PW6_FILE_LOC "./pwd6sha256"



void readFileIntoString(const char* file_loc, BYTE hashed_pws[], size_t size);
void generateHash(const BYTE *to_hash, BYTE hash[]);
void printPWListAsHex(BYTE pw_hashes[], size_t size);
int pwEqualToListAt(char *guess, BYTE pw_hashes[], size_t size);
void crackPasswordsFromFile(const char* guess_file, const char* hashed_file);
void generateNGuesses(int n);

// Password generation functions
void allNDigitNums(int n, BYTE hashed_pws[], size_t size);

// 0 arg: generate guesses and test them against sha256 hashes
// 1 arg: int: how many guesses to produce, don't compare to hashes
// 2 arg: 1. file of password guesses, 2. file w/ list of sha256 hashes
int main(int argc, char const *argv[]) {

    int n_guesses;
    if (argc == 1) {
        // generate and guess, guess against what !!!!?????
        printf("No args supplied. Try crack passwords\n");
    } else if (argc == 2) {
        n_guesses = atoi(argv[1]);
        printf("1 arg supplied. Generate %d passwords\n", n_guesses);
        // generate n guesses
        generateNGuesses(n_guesses);
    } else if(argc == 3) {
        printf("2 arg supplied. Compare list of passwords to hashed ones\n");
        crackPasswordsFromFile(argv[1], argv[2]);
    }//

    return 0;
}

// prints n guesses to stdout
void generateNGuesses(int n) {
    printf("Printing %d guesses\n", n);
    // tracks current guess number in all subfunctions
    int curr_guess = 0;
    printf("The current guess is: %d\n", curr_guess);
}

// two argument form
void crackPasswordsFromFile(const char* guess_file, const char* hashed_file) {
    struct stat st;
    stat(hashed_file, &st);
    long size = st.st_size;
    // int num_hashes_in_file = size / SHA256_BLOCK_SIZE;
    BYTE pw_hashes[size];
    readFileIntoString(hashed_file, pw_hashes, size);
    printf("crack passwords from file, print the hashed file\n");
    printPWListAsHex(pw_hashes, size);
}

void allNDigitNums(int n, BYTE hashed_pws[], size_t size) {
    int i=0;
    while (i<pow(10, n)) {
        if (n==4) {
            char str[5];
            sprintf(str, "%04d", i);
            pwEqualToListAt(str, hashed_pws, size);
        } else if (n==6) {
            char str[7];
            sprintf(str, "%06d", i);
            pwEqualToListAt(str, hashed_pws, size);
        } else {
            printf("Not supported\n");
        }
        i++;
    }
}

/*
 * Takes guess as String INCLUDING NULL BYTE
 * compares single guess to all passwords in the given list
 * Arguments: guess: raw character string of guess
              pw_hashes[]: byte array containing hashed pws to cracks
 * Return: -1 if no match, 1 indexed return val
*/
int pwEqualToListAt(char *guess, BYTE pw_hashes[], size_t size) {
    BYTE hash_guess[SHA256_BLOCK_SIZE];
    generateHash((const BYTE *) guess, hash_guess);

    BYTE temp[SHA256_BLOCK_SIZE];
    memset(temp, '\0', SHA256_BLOCK_SIZE);
    int hashes_read = 0, i_temp = 0;

    for (int i=0; i<size; ++i) {
        // i and modulo
        i_temp = i - (hashes_read * SHA256_BLOCK_SIZE);
        memcpy(&temp[i_temp], &pw_hashes[i], sizeof(BYTE));
        if ((i+1)%SHA256_BLOCK_SIZE == 0) {
            hashes_read++;
            int ret_memcmp;
            if (!(ret_memcmp = memcmp(temp, hash_guess, SHA256_BLOCK_SIZE))) {
                printf("%s %d\n", guess, hashes_read);
                memset(temp, '\0', SHA256_BLOCK_SIZE);
                return hashes_read;
            }
        }
    }
    // No hit, return -1
    return -1;
}



// Source: https://stackoverflow.com/questions/36381509/how-to-print-sha512-hash-in-c
void printPWListAsHex(BYTE pw_hashes[], size_t size) {
    printf("0: ");
    for(int i = 0; i < size; ++i) {
        printf("%02x", pw_hashes[i]);
        if ((i+1)%(SHA256_BLOCK_SIZE) == 0 && i != size-1) {
            printf("\n%d: ", (i+1)/SHA256_BLOCK_SIZE);
        }
    }
    printf("\n");
}

void generateHash(const BYTE *to_hash, BYTE hash[]) {
    SHA256_CTX sha_ctx;
    sha256_init(&sha_ctx);
    sha256_update(&sha_ctx, to_hash, strlen((const char *)to_hash));
    sha256_final(&sha_ctx, hash);
}


void readFileIntoString(const char* file_loc, BYTE hashed_pws[], size_t size) {
    FILE *fptr;

    /*  open the file for reading */
    fptr = fopen(file_loc, "r");

    if (fptr == NULL) {
        printf("Cannot open file \n");
        exit(0);
    }

    fread(hashed_pws, size, 1, fptr);

    fclose(fptr);
}
