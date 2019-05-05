/**
* Password cracker - project 2 Comp30023
* Author: Kyle Zsembery
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "sha256.h"

// bytes i.e. 32*8 chars
#define PW4_FILE_LOC "./pwd4sha256"


void readFileIntoString(char* file_loc, BYTE hashed_pws[], size_t size);
BYTE* generateHash(const BYTE *to_hash, BYTE hash[]);
void printPWListAsHex(BYTE pw_hashes[], size_t size);
int pwEqualToListAt(char *guess, BYTE pw_hashes[]);

int main(int argc, char const *argv[]) {

    struct stat st;
    stat(PW4_FILE_LOC, &st);
    long size = st.st_size;
    // int num_hashes_in_file = size / SHA256_BLOCK_SIZE;
    BYTE pw4_hashes[size];
    readFileIntoString(PW4_FILE_LOC, pw4_hashes, size);

    char word[] = "busym";
    int list_index = pwEqualToListAt(word, pw4_hashes);


    // Begin cracking passwords... separate function


    return 0;
}

/*
 * Takes guess as String INCLUDING NULL BYTE
 * compares single guess to all passwords in the given list
 * Arguments: guess: raw character string of guess
              pw_hashes[]: byte array containing hashed pws to cracks
 * Return: -1 if no match, 0 indexed return val
*/
int pwEqualToListAt(char *guess, BYTE pw_hashes[]) {
    BYTE hash_guess[SHA256_BLOCK_SIZE];
    generateHash(guess, hash_guess);

    BYTE first_pass[SHA256_BLOCK_SIZE];
    memcpy(first_pass, pw_hashes, SHA256_BLOCK_SIZE);

    printf("hash guess as hex: \n");
    printPWListAsHex(hash_guess, SHA256_BLOCK_SIZE);

    printPWListAsHex(pw_hashes, 320);

    int retVal =  memcmp(hash_guess, first_pass, SHA256_BLOCK_SIZE);
    printf("memcpy retVal: %d\n", retVal);

    return 0;

    // divide list
    // loop and compare to each item in list
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

//
BYTE* generateHash(const BYTE *to_hash, BYTE hash[]) {
    printf("First hash of word inside hash function\n");
    SHA256_CTX sha_ctx;
    sha256_init(&sha_ctx);
    sha256_update(&sha_ctx, to_hash, strlen(to_hash));
    sha256_final(&sha_ctx, hash);
}


void readFileIntoString(char* file_loc, BYTE hashed_pws[], size_t size) {
    FILE *fptr;
    char ch;

    /*  open the file for reading */
    fptr = fopen(file_loc, "r");

    if (fptr == NULL) {
        printf("Cannot open file \n");
        exit(0);
    }

    fread(hashed_pws, size, 1, fptr);

    fclose(fptr);
}
