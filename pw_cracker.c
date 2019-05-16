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

#define CHAR_PWS_TESTED 6
#define PW4_FILE_LOC "./pwd4sha256"
#define PW6_FILE_LOC "./pwd6sha256"


void readFileIntoString(const char* file_loc, BYTE pw_hashes[], size_t size);
void generateHash(const BYTE *to_hash, BYTE hash[]);
void printPWListAsHex(BYTE pw_hashes[], size_t size);
int pwEqualToListAt(char *guess, BYTE pw_hashes[], size_t size);
void crackPasswordsFromFile(const char* guess_file, const char* hashed_file);
void generatePasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size);

void generate4CharPasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size);
void generate6CharPasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size);

// Password generation functions/strategies
/*
    void strategy(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);
*/
// Both
void nDigitNums(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);

// 4 char
void bruteForce4CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);

// 6 char
void bruteForce6CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);

// 0 arg: generate guesses and test them against sha256 hashes
// 1 arg: int: how many guesses to produce, don't compare to hashes
// 2 arg: 1. file of password guesses, 2. file w/ list of sha256 hashes
int main(int argc, char const *argv[]) {
    int n_guesses;
    struct stat st;
    if (argc == 1) {
        // generate and guess, guess against what !!!!?????
        printf("No args supplied. Try crack passwords\n");
        const char* hashed_file;
        if (CHAR_PWS_TESTED == 4) {
            hashed_file = PW4_FILE_LOC;
        } else if (CHAR_PWS_TESTED == 6) {
            hashed_file = PW6_FILE_LOC;
        }
        stat(hashed_file, &st);
        long size = st.st_size;
        BYTE pw_hashes[size];
        readFileIntoString(hashed_file, pw_hashes, size);
        generatePasswords(true, -1, pw_hashes, size);
    } else if (argc == 2) {
        n_guesses = atoi(argv[1]);
        printf("1 arg supplied. Generate %d passwords\n", n_guesses);
        // generate n guesses
        // with crack false, pw_hashes never need to be used.
        generatePasswords(false, n_guesses, NULL, 0);
    } else if(argc == 3) {
        printf("2 arg supplied. Compare list of passwords to hashed ones\n");
        crackPasswordsFromFile(argv[1], argv[2]);
    }
    return 0;
}

// prints n guesses to stdout or attempts to use guess to crack
void generatePasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size) {
    // Generate guesses of length CHAR_PWS_TESTED
    if (CHAR_PWS_TESTED == 4) {
        printf("4 character passwords being generated\n");
        generate4CharPasswords(crack, n, pw_hashes, pw_size);
    } else if (CHAR_PWS_TESTED == 6) {
        printf("6 character passwords being generated\n");
        generate6CharPasswords(crack, n, pw_hashes, pw_size);
    } else {
        printf("Invalid value set for CHAR_PWS_TESTED\n");
    }
}

void generate4CharPasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size) {
    // tracks current guess number in all subfunctions
    int curr_guess = 0;
    printf("Generating %d 4 char passwords, cracking? %d\n", n, crack);
    nDigitNums(crack, pw_hashes, pw_size, n, &curr_guess);
    bruteForce4CharAlpha(crack, pw_hashes, pw_size, n, &curr_guess);
    // All 4 char password generation strategies
}

void generate6CharPasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size) {
    // tracks current guess number in all subfunctions
    int curr_guess = 0;
    printf("Generating %d 6 char passwords, cracking? %d\n", n, crack);
    // All 6 char password generation strategies
    bruteForce6CharAlpha(crack, pw_hashes, pw_size, n, &curr_guess);
}

/* Brute force generate all passwords with alphabetic characters.
* Start with lower case. Then upper first letter, upper second etc.
*/
void bruteForce4CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess) {
    int c1, c2, c3, c4;
    for (c1 = 65; c1<123; c1++) {
        if (c1 > 90 && c1 < 97) {
            continue;
        }
        for (c2 = 65; c2<123; c2++) {
            if (c2 > 90 && c2 < 97) {
                continue;
            }
            for (c3 = 65; c3<123; c3++) {
                if (c3 > 90 && c3 < 97) {
                    continue;
                }
                for (c4 = 65; c4<123; c4++) {
                    if (c4 > 90 && c4 < 97) {
                        continue;
                    }
                    if (*curr_guess < n) {
                        printf("%c%c%c%c\n", c1, c2, c3, c4);
                        *curr_guess += 1;
                    } else if (n == -1 && crack) {
                        char str[5];
                        sprintf(str, "%c%c%c%c", c1, c2, c3, c4);
                        pwEqualToListAt(str, pw_hashes, pw_size);
                    }
                }
            }
        }
    }
}

/* */
void bruteForce6CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess) {
    FILE *out_file = fopen("bruteForce6Char.txt", "w+"); // write only
    FILE *answers = fopen("answers.txt", "w+"); // write only
    int c1, c2, c3, c4, c5, c6;
    for (c1 = 90; c1<123; c1++) {
        if (c1 > 90 && c1 < 97) {
            continue;
        }
        for (c2 = 65; c2<123; c2++) {
            if (c2 > 90 && c2 < 97) {
                continue;
            }
            for (c3 = 65; c3<123; c3++) {
                if (c3 > 90 && c3 < 97) {
                    continue;
                }
                for (c4 = 65; c4<123; c4++) {
                    if (c4 > 90 && c4 < 97) {
                        continue;
                    }
                    for (c5 = 65; c5<123; c5++) {
                        if (c5 > 90 && c5 < 97) {
                            continue;
                        }
                        for (c6 = 65; c6<123; c6++) {
                            if (c6 > 90 && c6 < 97) {
                                continue;
                            }
                            if (*curr_guess < n) {
                                printf("%c%c%c%c%c%c\n", c1, c2, c3, c4, c5, c6);
                                *curr_guess += 1;
                            } else if (n == -1 && crack) {
                                char str[7];
                                sprintf(str, "%c%c%c%c%c%c", c1, c2, c3, c4, c5, c6);
                                int index = pwEqualToListAt(str, pw_hashes, pw_size);
                                if (index > 0) {
                                    fprintf(answers, "Correct: %s, hash: %d\n", str, index);
                                }
                                // printf("Curr guess: %d\n", *curr_guess);
                                if ((*curr_guess % 3000000) == 0) {
                                    fprintf(out_file, "Guess %d: %s\n", *curr_guess, str); // write to file
                                }
                                *curr_guess += 1;
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(out_file);
    fclose(answers);
}

void nDigitNums(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess) {
    int i=0;
    while (i<pow(10, CHAR_PWS_TESTED) && (*curr_guess < n || n == -1)) {
        if (CHAR_PWS_TESTED==4) {
            char str[5];
            sprintf(str, "%04d", i);
            if (crack) {
                pwEqualToListAt(str, pw_hashes, pw_size);
            } else {
                printf("%04d\n", i);
            }
        } else if (CHAR_PWS_TESTED==6) {
            char str[7];
            sprintf(str, "%06d", i);
            if (crack) {
                pwEqualToListAt(str, pw_hashes, pw_size);
            } else {
                printf("%04d\n", i);
            }
        } else {
            printf("Not supported\n");
        }
        i++;
        *curr_guess += 1;
    }
}

// Two command line arguments, this is called.
void crackPasswordsFromFile(const char* guess_file, const char* hashed_file) {
    struct stat st;
    stat(hashed_file, &st);
    long pw_size = st.st_size;
    // int num_hashes_in_file = size / SHA256_BLOCK_SIZE;
    BYTE pw_hashes[pw_size];
    readFileIntoString(hashed_file, pw_hashes, pw_size);
    FILE *guesses_fp = fopen(guess_file, "r");
    if (guesses_fp == NULL){
      printf("Could not open file %s", guess_file);
      exit(1);
    }
    // loop through guesses, comparing each to the hashes
    char guess_raw[50];
    while (fgets(guess_raw, 50, guesses_fp) != NULL) {
        int len_guess = strlen(guess_raw);
        guess_raw[len_guess-1] = '\0';
        char guess[len_guess];
        sprintf(guess, "%s", guess_raw);
        pwEqualToListAt(guess, pw_hashes, pw_size);
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


void readFileIntoString(const char* file_loc, BYTE pw_hashes[], size_t size) {
    FILE *fptr;

    /*  open the file for reading */
    fptr = fopen(file_loc, "r");

    if (fptr == NULL) {
        printf("Cannot open file \n");
        exit(0);
    }

    fread(pw_hashes, size, 1, fptr);

    fclose(fptr);
}
