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

int main(int argc, char const *argv[]) {

    // ====== * this works, testing
    char pw[9] = "password";
    BYTE hash[SHA256_BLOCK_SIZE];
    generateHash(pw, hash);
    // ===================





    // print as hex
    //Source https://stackoverflow.com/questions/36381509/how-to-print-sha512-hash-in-c
    for(int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
       printf("%02x", hash[i]);
    }
    printf("\n");

    struct stat st;
    stat(PW4_FILE_LOC, &st);
    long size = st.st_size;
    // int num_hashes_in_file = size / SHA256_BLOCK_SIZE;
    BYTE pw4_hashes[size];
    readFileIntoString(PW4_FILE_LOC, pw4_hashes, size);
    printf("About to start printing pw4 hashes\n");
    printf("0: ");
    for(int i = 0; i < size; ++i) {
        printf("%02x", pw4_hashes[i]);
        if ((i+1)%(SHA256_BLOCK_SIZE) == 0) {
            printf("\n%d: ", (i+1)/SHA256_BLOCK_SIZE);
        }
    }
    printf("\n");
    // Begin cracking passwords... separate function


    return 0;
}

//
BYTE* generateHash(const BYTE *to_hash, BYTE hash[]) {
    SHA256_CTX sha_ctx;
    sha256_init(&sha_ctx);
    sha256_update(&sha_ctx, to_hash, sizeof(to_hash));
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
    // BYTE hash[SHA256_BLOCK_SIZE];
    fread(hashed_pws, size, 1, fptr);




    // fwrite(fptr, 32, 10, stdout);
    // printf(str_ptr);

    fclose(fptr);
}
