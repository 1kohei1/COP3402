#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#define DEFAULT_INPUT_FILE "input.pl0"
#define FILE_NAME_WITHOUT_COMMENT "source_without_comment.pl0"

int shouldPrintSource = 0;
int shouldPrintSourceWithoutComment = 0;

void emptySourceWithoutCommentFile();
void removeCommentFromInputFile(char* inputFileName);

// Utility functions
void printFile(char* header, char* fileName);
FILE* openFile(char* fileName, char* mode);

int main(int argc, char** argv) {
    char inputFileName[100];
    // Get input file name. If it is executed without any args, use default input file
    if (argc == 1) {
        strcpy(inputFileName, DEFAULT_INPUT_FILE);
    } 
    // If some options are defined, get file name, and --source and --clean parameter
    else {
        strcpy(inputFileName, argv[1]);
        int i;
        for (i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--source") == 0) {
                shouldPrintSource = 1;
            } else if (strcmp(argv[i], "--clean") == 0) {
                shouldPrintSourceWithoutComment = 1;
            }
        }
    }
    
    // Make source without comment file empty
    emptySourceWithoutCommentFile();
    
    // Remove comments from input file so that we can handle print source without comment arg.
    removeCommentFromInputFile(inputFileName);
    
    // Handle parameters
    if (shouldPrintSource) {
        printFile("source code:", inputFileName);
    }
    if (shouldPrintSourceWithoutComment) {
        printFile("source code without comments:", FILE_NAME_WITHOUT_COMMENT);
    }
    
}

void emptySourceWithoutCommentFile() {
    FILE* f = openFile(FILE_NAME_WITHOUT_COMMENT, "w");
    fclose(f);
}

void removeCommentFromInputFile(char* inputFileName) {
    FILE* input = openFile(inputFileName, "r");
    FILE* output = openFile(FILE_NAME_WITHOUT_COMMENT, "w");

    // Read first two characters.
    char prev, curr;
    if (fscanf(input, "%c%c", &prev, &curr) == EOF) {
        return;
    }
    
    // Put previous character
    fputc(prev, output);
    
    int isInComment = 0;
    
    while (fscanf(input, "%c", &curr) != EOF) {
        printf("prev: %c, curr: %c\n", prev, curr);
        if (prev == '/' && curr == '*') {
            isInComment = 1;
        } else if (prev == '*' && curr == '/') {
            // To replace '*' and '/' with empty character, set empty character to prev and curr
            prev = ' ';
            curr = ' ';
            isInComment = 0;
        }
        
        int result = fputc(isInComment ? ' ' : prev, output);
        if (result == EOF) {
            printf("[ERR] somehow, failed to write program without comment to %s\n", FILE_NAME_WITHOUT_COMMENT);
            exit(1);
        }
        // Update previous charcter
        prev = curr;
    }
    
    // Close the file
    fclose(input);
    fclose(output);
}


void printFile(char* header, char* fileName) {
    // Opens the file. If file is not found, exit the program
    FILE* f = openFile(fileName, "r");
    
    // Print hedaer
    printf("%s\n", header);
    int i;
    for (i = 0; i < strlen(header); i++) {
        printf("-");
    }
    printf("\n");
    
    char* line;
    while (fgets(line, 256, f)) {
        printf("%s\n", line);
    }
    
    // Close the file
    fclose(f);
}

/**
 * Open file for read with given file name. 
 * If it failed to open given file, it ends a program.
 */
FILE* openFile(char* fileName, char* mode) {
    // Opens the file. If file is not found, exit the program
    FILE* f;
    f = fopen(fileName, mode);
    if (f == NULL) {
        printf("[ERR] Failed to open file %s\n", fileName);
        exit(1);
    }
    
    return f;
}