/**
* Password cracker - project 2 Comp30023
* Author: Kyle Zsembery
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// bytes i.e. 32*8 chars
#define PW_LEN_SHA256_BYTES 32
#define PW4_FILE_LOC "./pwd4sha256"

char* getPasswordAtIndex(int i, char* sha256_string);
char* readFileIntoString(char*file_loc);

int main(int argc, char const *argv[]) {


    readFileIntoString(PW4_FILE_LOC);
    // printf("%s", sha256_string);

    // Begin cracking passwords... separate function

    // getPasswordAtIndex(0, sha256_string);


    return 0;
}

// Gets a specific password from the list by index
char* getPasswordAtIndex(int i, char* sha256_string) {
    int start = i * PW_LEN_SHA256_BYTES;
    int end = start + PW_LEN_SHA256_BYTES;
}


char* readFileIntoString(char* file_loc) {
    FILE *fptr;
    char ch;

    char* sha256_string = malloc(sizeof(char) * 320*8);


    /*  open the file for reading */
    fptr = fopen(file_loc, "r");
    struct stat st;
    stat(file_loc, &st);
    long size = st.st_size;
    printf("Size of file loc file: %ld\n", size);
    if (fptr == NULL) {
        printf("Cannot open file \n");
        exit(0);
    }

    // fwrite(sha256_string, 32, 10, fptr);
    // printf(str_ptr);

    fclose(fptr);

    return "no";
}
