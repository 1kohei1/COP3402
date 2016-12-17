#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INPUT_FILE "input.pl0"
#define FILE_NAME_WITHOUT_COMMENT "source_without_comment.pl0"
#define TOKEN_LIST_FILE "token_list.txt"

int shouldPrintSource = 0;
int shouldPrintSourceWithoutComment = 0;

void emptySourceWithoutCommentFile();
void removeCommentFromInputFile(char* inputFileName);
void lexicalAnalysis();

// Utility functions
void printFile(char* header, char* fileName);
FILE* openFile(char* fileName, char* mode);
int isDigit(char c);
int isLetter(char c);

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
    
    // Do lexical analysis and output result to TOKEN_LIST
    lexicalAnalysis();
    
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
    // Flag for if it is in comment or not.
    int isInComment = 0;
    
    while (fscanf(input, "%c", &curr) != EOF) {
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

void lexicalAnalysis() {
    FILE* input = openFile(FILE_NAME_WITHOUT_COMMENT, "r");
    FILE* output = openFile(TOKEN_LIST_FILE, "w");
    
    char curr;
    while(fscanf(input, "%c", &curr) != EOF) {
        // If space, we don't need it. Keep going
        if      (curr == ' ') continue;
        // Handle one letter token
        else if (curr == '+') fprintf(output, "%s %d ", "+", 4);
        else if (curr == '-') fprintf(output, "%s %d ", "-", 5);
        else if (curr == '*') fprintf(output, "%s %d ", "*", 6);
        else if (curr == '/') fprintf(output, "%s %d ", "/", 7);
        else if (curr == '=') fprintf(output, "%s %d ", "=", 9);
        else if (curr == '(') fprintf(output, "%s %d ", "(", 15);
        else if (curr == ')') fprintf(output, "%s %d ", ")", 16);
        else if (curr == ',') fprintf(output, "%s %d ", ",", 17);
        else if (curr == ';') fprintf(output, "%s %d ", ";", 18);
        else if (curr == '.') fprintf(output, "%s %d ", ".", 19);
        // Handle multiple letter tokens
        else if (curr == ':') {
            // Get next character
            char next;
            fscanf(input, "%c", &next);
            
            if (next == '=') {
                fprintf(output, "%s %d ", ":=", 20);
            } else {
                printf("[ERR] Invalid token (:%c) is given\nEnd the program\n", next);
                exit(1);
            }
        }
        else if (curr == '<' || curr == '>') {
            // Get next character
            char next;
            fscanf(input, "%c", &next);
            
            if      (curr == '<' && next == '>') fprintf(output, "%s %d ", "<>", 10);
            else if (curr == '<' && next == '=') fprintf(output, "%s %d ", "<=", 12);
            else if (curr == '>' && next == '=') fprintf(output, "%s %d ", ">=", 14);
            else if (curr == '<' && isDigit(next) && isLetter(next)) {
                // Register as '<' and let next cycle figure out what comes after this.
                fprintf(output, "%s %d ", "<", 11);
                // Move cursor one back to handle next token.
                fseek(input, 1, SEEK_CUR);
            } else if (curr == '>' && isDigit(next) && isLetter(next)) {
                // Register as '<' and let next cycle figure out what comes after this.
                fprintf(output, "%s %d ", ">", 13);
                // Move cursor one back to handle next token.
                fseek(input, 1, SEEK_CUR);
            }
            else {
                printf("[ERR] Invalid token (%c%c) is given\nEnd the program\n", curr, next);
                exit(1);
            }
        }
        // Now everything left is something we need to read till space
        else {
            
        }
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

int isDigit(char c) {
    return '0' <= c && c <= '9';
}

int isLetter(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}