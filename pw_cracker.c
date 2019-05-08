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

// bytes i.e. 32*8 chars
#define PW4_FILE_LOC "./pwd4sha256"


void readFileIntoString(char* file_loc, BYTE hashed_pws[], size_t size);
void generateHash(const BYTE *to_hash, BYTE hash[]);
void printPWListAsHex(BYTE pw_hashes[], size_t size);
int pwEqualToListAt(char *guess, BYTE pw_hashes[], size_t size);
void crackPasswords(BYTE hashed_pws[], size_t size);

// Password generation functions
void allNDigitNums(int n, BYTE hashed_pws[], size_t size);

int main(int argc, char const *argv[]) {

    struct stat st;
    stat(PW4_FILE_LOC, &st);
    long size = st.st_size;
    // int num_hashes_in_file = size / SHA256_BLOCK_SIZE;
    BYTE pw4_hashes[size];
    readFileIntoString(PW4_FILE_LOC, pw4_hashes, size);


    // Begin cracking passwords... separate function
    crackPasswords(pw4_hashes, size);

    return 0;
}

void crackPasswords(BYTE hashed_pws[], size_t size) {
    allNDigitNums(4, hashed_pws, size);
    allNDigitNums(6, hashed_pws, size);
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


void readFileIntoString(char* file_loc, BYTE hashed_pws[], size_t size) {
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
