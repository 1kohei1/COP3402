#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDENT_LENGTH_LIMIT 12
#define DEFAULT_INPUT_FILE "input.pl0"
#define FILE_NAME_WITHOUT_COMMENT "source_without_comment.pl0"
#define TOKEN_LIST_FILE "token_list.txt"

int shouldPrintSource = 0;
int shouldPrintSourceWithoutComment = 0;

void emptySourceWithoutCommentFile();
void removeCommentFromInputFile(char* inputFileName);
void lexicalAnalysis();
void printTokenList();

// Utility functions
void printFile(char* header, char* fileName);
FILE* openFile(char* fileName, char* mode);
int isDigit(char c);
int isLetter(char c);
char* readUptoNonLetterNonDigit(FILE* f);
char* appendChar(char* str, char c);
int isValidIdent(char* ident);
void checkNextCharIfNotEndProgram(FILE* f, char* token, char c);
void consumeChar(FILE* f, char c);

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

    // Output token list
    printTokenList();
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
    prev = curr;
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
        // If space or new line, ignore it.
        if (curr == ' ' || curr == '\n') continue;
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
            else if (curr == '<' && (isDigit(next) || isLetter(next))) {
                // Register as '<' and let next cycle figure out what comes after this.
                fprintf(output, "%s %d ", "<", 11);
                // Move cursor one back to handle next token.
                fseek(input, -1, SEEK_CUR);
            } else if (curr == '>' && (isDigit(next) || isLetter(next))) {
                // Register as '<' and let next cycle figure out what comes after this.
                fprintf(output, "%s %d ", ">", 13);
                // Move cursor one back to handle next token.
                fseek(input, -1, SEEK_CUR);
            }
            else {
                printf("[ERR] Invalid token (%c%c) is given\nEnd the program\n", curr, next);
                exit(1);
            }
        }
        // Now everything left is something we need to read till space
        else {
            // Move cursor one back to get token.
            fseek(input, -1, SEEK_CUR);
            
            char* token = readUptoNonLetterNonDigit(input);
            
            if      (strcmp(token, "begin") == 0) fprintf(output, "%s %d ", token, 21);
            else if   (strcmp(token, "end") == 0) fprintf(output, "%s %d ", token, 22);
            else if    (strcmp(token, "if") == 0) fprintf(output, "%s %d ", token, 23);
            else if  (strcmp(token, "then") == 0) fprintf(output, "%s %d ", token, 24);
            else if (strcmp(token, "while") == 0) fprintf(output, "%s %d ", token, 25);
            else if    (strcmp(token, "do") == 0) fprintf(output, "%s %d ", token, 26);
            else if  (strcmp(token, "else") == 0) fprintf(output, "%s %d ", token, 33);
            // Handle the case that needs to read next identifier
            else if  (strcmp(token, "call") == 0 ||
                     strcmp(token, "const") == 0 ||
                       strcmp(token, "var") == 0 ||
                 strcmp(token, "procedure") == 0 ||
                     strcmp(token, "write") == 0 ||
                      strcmp(token, "read") == 0) {
                // Check next character is space
                checkNextCharIfNotEndProgram(input, token, ' ');
            
                // Consume extra space after the first space.
                consumeChar(input, ' ');
            
                // Get ident
                char *ident = readUptoNonLetterNonDigit(input);
                // Check if it is a valid identifier.
                if (!isValidIdent(ident)) {
                    printf("[ERR] Invalid ident (%s) is given\nEnd the program\n", ident);
                    exit(1);
                }
                
                // Get token value
                int value = 0;
                if           (strcmp(token, "call") == 0) value = 27;
                else if     (strcmp(token, "const") == 0) value = 28;
                else if       (strcmp(token, "var") == 0) value = 29;
                else if (strcmp(token, "procedure") == 0) value = 30;
                else if     (strcmp(token, "write") == 0) value = 31;
                else if      (strcmp(token, "read") == 0) value = 32;
                
                // Put token
                fprintf(output, "%s %d ", token, value);
                // Put ident
                fprintf(output, "%s %d ", ident, 2);
                
                // When it is const, it requires = number. Make sure they appear after const ident.
                if (strcmp(token, "const") == 0) {
                    
                }
                
                else if (strcmp(token, "var") == 0) {
                    
                }
            }
            // Invalid token
            else {
                // printf("[ERR] Invalid token (%s) is given\nEnd the program\n", token);
                // exit(1);
            }
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

void printTokenList() {
    FILE* tokenList = openFile(TOKEN_LIST_FILE, "r");
    
    printf("tokens:\n");
    printf("-------\n");
    
    char* token;
    int value;
    
    while (fscanf(tokenList, "%s %d ", token, &value) != EOF) {
        printf("%-20s%d\n", token, value);
    }
    
    fclose(tokenList);
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

char* readUptoNonLetterNonDigit(FILE* f) {
    char c;
    char* str = malloc(sizeof(char));
    
    while (fscanf(f, "%c", &c) != EOF && (isDigit(c) || isLetter(c))) {
        str = appendChar(str, c);
    }
    
    // The last character was not digit or letter. 
    // To read that character in the next loop, go back 1 character.
    fseek(f, -1, SEEK_CUR);
    
    return str;
}

char* appendChar(char* str, char c) {
    char* returnS = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(returnS, str);
    returnS[strlen(str)] = c;
    
    return returnS;
}

int isDigit(char c) {
    return '0' <= c && c <= '9';
}

int isLetter(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int isValidIdent(char* ident) {
    int len = strlen(ident);
    // Check first character is a letter
    if (!isLetter(ident[0])) {
        return 0;
    }
    // Check length
    if (len > IDENT_LENGTH_LIMIT) {
        return 0;
    }
    
    // Check all other part is consisted by digit or letter
    int i;
    for (i = 1; i < len; i++) {
        if (!(isDigit(ident[i]) || isLetter(ident[i]))) {
            return 0;
        }
    }
    
    return 1;
}

void checkNextCharIfNotEndProgram(FILE* f, char* token, char c) {
    char temp;
    if (fscanf(f, "%c", &temp) == EOF || temp != c) {
        printf("[ERR] Invalid token (%s%c) is given\nEnd the program\n", token, temp);
        exit(1);
    }
}

/**
 * Consume given c until, it hits other than c
 */
void consumeChar(FILE* f, char c) {
    char temp;
    while (fscanf(f, "%c", &temp) != EOF && temp == c) {
        // Do nothing
    }
    // Move cursor 1 back since we read character that is not equal to given c.
    fseek(f, -1, SEEK_CUR);
}
