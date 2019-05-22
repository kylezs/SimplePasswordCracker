/**
* Password cracker - project 2 Comp30023
* Author: Kyle Zsembery
*
0 arg: generate guesses and test them against sha256 hashes
1 arg: int: how many guesses to produce, don't compare to hashes
2 arg: 1. file of password guesses, 2. file w/ list of sha256 hashes
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include "sha256.h"

#define CHAR_PWS_TESTED 6
#define PW4_FILE_LOC "./pwd4sha256"
#define PW6_FILE_LOC "./pwd6sha256"
#define CRACKED_PREV "cracked_prev.txt"
#define COMMON_PWS "proj-2_common_passwords.txt"
// #define COMMON_PWS "testpw.txt"

#define START_CHAR_SET 32
#define END_CHAR_SET 126
#define UPPER_START 65
#define UPPER_END 90
#define LOWER_START 97
#define LOWER_END 123

#define LOG_EVERY_GUESS_AT 5000000

void readFileIntoString(const char* file_loc, BYTE pw_hashes[], size_t size);
void generateHash(const BYTE *to_hash, BYTE hash[]);
void printPWListAsHex(BYTE pw_hashes[], size_t size);
int pwEqualToListAt(char *guess, BYTE pw_hashes[], size_t size);
void crackPasswordsFromFile(const char* guess_file, const char* hashed_file);
void generatePasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size);

void generate4CharPasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size);
void generate6CharPasswords(bool crack, int n, BYTE pw_hashes[], size_t pw_size);


// Both
void nDigitNums(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);

// 4 char
void bruteForce4CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);

// 6 char
void bruteForce6CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);
void varyAlreadyGuessed(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);
void varyCommonPasswords(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess);
void varyGuess(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess, char* guess);


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

    varyCommonPasswords(crack, pw_hashes, pw_size, n, &curr_guess);
    bruteForce6CharAlpha(crack, pw_hashes, pw_size, n, &curr_guess);
    varyAlreadyGuessed(crack, pw_hashes, pw_size, n, &curr_guess);

}

/* only takes 6 letter guesses in CRACKED_PREV */
void varyAlreadyGuessed(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess) {
    FILE *cracked_prev = fopen(CRACKED_PREV, "r");
    if (cracked_prev == NULL){
      printf("Could not open file %s", CRACKED_PREV);
      exit(1);
    }
    // loop through guesses, comparing each to the hashes
    char guess[7];
    while (fgets(guess, 7, cracked_prev) != NULL) {
        if (strncmp(guess, "\n", 1) == 0) {
            continue;
        }
        guess[6] = '\0';
        // modify the guess with smart variations
        varyGuess(crack, pw_hashes, pw_size, n, curr_guess, guess);
    }
}

void varyCommonPasswords(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess) {
    printf("Varying common passwords\n");
    FILE *common_pws = fopen(COMMON_PWS, "r");
    if (common_pws == NULL){
      printf("Could not open file %s", COMMON_PWS);
      exit(1);
    }
    // loop through guesses, comparing each to the hashes
    char guess_raw[50];
    while (fgets(guess_raw, sizeof(guess_raw), common_pws) != NULL) {
        if (strncmp(guess_raw, "\n", 1) == 0) {
            continue;
        }
        // stop newline from being counted as a char in the string
        int len_guess = strlen(guess_raw);
        if (guess_raw[len_guess-1] == '\n') {
            guess_raw[len_guess-1] = '\0';
        }
        len_guess = strlen(guess_raw);

        char guess[7];
        if (len_guess<6) {
            // pad the word at different parts if shorter than 6 chars
            char* c_point;
            // controls number of variations on padding
            for (int j=0; j<7-len_guess; j++) {
                c_point = &guess[j];
                // add paddings. Add 0 to end, and just leave first letter
                // while moving across
                if (j==0) {
                    for (int c_num=len_guess-1; c_num<6; c_num++) {
                        guess[c_num] = '0';
                    }
                }
                // add 0s as padding, will be replaced by varyGuess
                memcpy(c_point, guess_raw, len_guess);
                guess[6] = '\0';
                varyGuess(crack, pw_hashes, pw_size, n, curr_guess, guess);
                // printf("Guess from less than equal 6: %s\n", guess);
            }
        } else if (len_guess > 6) {
            // shift the word over, cutting diff bits off if longer than 6 chars
            for (int k=0; k<len_guess-5; k++) {
                memcpy(guess, &guess_raw[k], 6);
                guess[6] = '\0';
                varyGuess(crack, pw_hashes, pw_size, n, curr_guess, guess);
            }
        } else {
            sprintf(guess, "%s", guess_raw);
            varyGuess(crack, pw_hashes, pw_size, n, curr_guess, guess);
        }
    }
}

/* modifies the guess in numerous ways, comparing to hashes as it goes */
void varyGuess(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess, char* guess) {
    // try every valid character in place of the character that's already there
    // printf("Guess being varied: %s\n", guess);
    for (int i=0; i<6; i++) {
        int count = 0;
        char c_orig = guess[i];
        for (int new_c = START_CHAR_SET; new_c < END_CHAR_SET; new_c++) {
            guess[i] = (char) new_c;
            if (new_c != (int) c_orig) {
                count++;
                if (*curr_guess < n) {
                    printf("%s\n", guess);
                    *curr_guess += 1;
                } else if (n == -1 && crack) {
                    pwEqualToListAt(guess, pw_hashes, pw_size);
                }
            }
        }
        // put it back to original after all variations tried
        guess[i] = c_orig;
    }
}

/*
* Brute force generate all passwords with alphabetic characters.
*/
void bruteForce4CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess) {
    int c1, c2, c3, c4;
    for (c1 = UPPER_START; c1<LOWER_END; c1++) {
        if (c1 > UPPER_END && c1 < LOWER_START) {
            continue;
        }
        for (c2 = UPPER_START; c2<LOWER_END; c2++) {
            if (c2 > UPPER_END && c2 < LOWER_START) {
                continue;
            }
            for (c3 = UPPER_START; c3<LOWER_END; c3++) {
                if (c3 > UPPER_END && c3 < LOWER_START) {
                    continue;
                }
                for (c4 = UPPER_START; c4<LOWER_END; c4++) {
                    if (c4 > UPPER_END && c4 < LOWER_START) {
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

/* Currently, this just bruteforces lowercase passwords */
void bruteForce6CharAlpha(bool crack, BYTE pw_hashes[], size_t pw_size, int n, int *curr_guess) {
    FILE *out_file = fopen("bruteForce6Char.txt", "w+");
    FILE *answers = fopen("answers.txt", "w+");
    int c1, c2, c3, c4, c5, c6;
    for (c1 = LOWER_START; c1<LOWER_END; c1++) {
        for (c2 = LOWER_START; c2<LOWER_END; c2++) {
            for (c3 = LOWER_START; c3<LOWER_END; c3++) {
                for (c4 = LOWER_START; c4<LOWER_END; c4++) {
                    for (c5 = LOWER_START; c5<LOWER_END; c5++) {
                        for (c6 = LOWER_START; c6<LOWER_END; c6++) {
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
                                if ((*curr_guess % LOG_EVERY_GUESS_AT) == 0) {
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

/* Two command line arguments, this is called. The guess file must
* end with a newline
*/
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
