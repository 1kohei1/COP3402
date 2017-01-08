#include "lexer.h"

#define NUMBER_LIMIT 65535
#define IDENT_LENGTH_LIMIT 12
#define FILE_NAME_WITHOUT_COMMENT "source_without_comment.pl0"
#define TOKEN_LIST_FILE "token_list.txt"

void emptySourceWithoutCommentFile();
void removeCommentFromInputFile(char* inputFileName);
void lexicalAnalysis();

// Utility functions
FILE* openFile(char* fileName, char* mode);
int isDigit(char c);
int isLetter(char c);
char* readUptoNonLetterNonDigit(FILE* f);
char* appendChar(char* str, char c);
int isValidIdent(char* ident);
int isValidNumber(char* number);
int convertToInt(char* number);

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
            // Handle the case that next character is not '>', '='
            else {
                fprintf(output, "%c %d ", curr, curr == '<' ? 11 : 13);
                // Move cursor 1 back to read next token.
                fseek(input, -1, SEEK_CUR);
            }
        }
        // Handle number
        else if (isDigit(curr)) {
            // Move cursor one back to get a number.
            fseek(input, -1, SEEK_CUR);
            
            char* number = readUptoNonLetterNonDigit(input);
            if (!isValidNumber(number)) {
                printf("[ERR] Invalid number (%s) is given\nEnd the program\n", number);
                exit(1);
            }
            
            fprintf(output, "%s %d ", number, 3);
        }
        // Handle reserved words or identifier
        else if (isLetter(curr)) {
            // Move cursor one back to get token.
            fseek(input, -1, SEEK_CUR);
            
            char* token = readUptoNonLetterNonDigit(input);
            
            if            (strcmp(token, "odd") == 0) fprintf(output, "%s %d ", token, 8);
            else if     (strcmp(token, "begin") == 0) fprintf(output, "%s %d ", token, 21);
            else if       (strcmp(token, "end") == 0) fprintf(output, "%s %d ", token, 22);
            else if        (strcmp(token, "if") == 0) fprintf(output, "%s %d ", token, 23);
            else if      (strcmp(token, "then") == 0) fprintf(output, "%s %d ", token, 24);
            else if     (strcmp(token, "while") == 0) fprintf(output, "%s %d ", token, 25);
            else if        (strcmp(token, "do") == 0) fprintf(output, "%s %d ", token, 26);
            else if      (strcmp(token, "call") == 0) fprintf(output, "%s %d ", token, 27);
            else if     (strcmp(token, "const") == 0) fprintf(output, "%s %d ", token, 28);
            else if       (strcmp(token, "var") == 0) fprintf(output, "%s %d ", token, 29);
            else if (strcmp(token, "procedure") == 0) fprintf(output, "%s %d ", token, 30);
            else if     (strcmp(token, "write") == 0) fprintf(output, "%s %d ", token, 31);
            else if      (strcmp(token, "read") == 0) fprintf(output, "%s %d ", token, 32);
            else if      (strcmp(token, "else") == 0) fprintf(output, "%s %d ", token, 33);
            // If token is not one of reserved word, treat it as identifier
            else {
                if (!isValidIdent(token)) {
                    printf("[ERR] Invalid ident (%s) is given\nEnd the program\n", token);
                    exit(1);
                }
                fprintf(output, "%s %d ", token, 2);
            }
        }
        else {
            printf("[ERR] Invalid token (%c) is given\nEnd the program\n", curr);
            exit(1);
        }
    }
    
    // Close the file
    fclose(input);
    fclose(output);
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
    
    while (fscanf(f, "%c", &c) != EOF) {
        // If next read character is not either digit or letter, get out of the loop and move cursor 1 back.
        if (!(isDigit(c) || isLetter(c))) {
            fseek(f, -1, SEEK_CUR);
            break;
        }
        str = appendChar(str, c);
    }
    
    return str;
}

char* appendChar(char* str, char c) {
    char* returnS = malloc(sizeof(char) * (strlen(str) + 1));
    sprintf(returnS, "%s%c", str, c);
    
    return returnS;
}

int isDigit(char c) {
    return '0' <= c && c <= '9';
}

int isLetter(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int isValidNumber(char* number) {
    int len = strlen(number);
    // If it is empty, it's not a valid number
    if (len == 0) {
        return 0;
    }
    int i;
    
    // Check each of them are digit
    for (i = 0; i < len; i++) {
        if (!isDigit(number[i])) {
            return 0;
        }
    }
    
    // Check the range.
    return convertToInt(number) <= NUMBER_LIMIT;
}

/**
 * Convert given number string to integer. number must be string that's consisted of digits.
 * It stops calculation when num gets greater than NUMBER_LIMIT.
 * This is to prevent integer overflow.
 */
int convertToInt(char* number) {
    int num = 0;
    int i;
    
    for (i = 0; i < strlen(number) && num <= NUMBER_LIMIT; i++) {
        num *= 10;
        num += number[i] - '0';
    }
    
    return num;
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
