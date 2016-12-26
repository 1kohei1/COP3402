#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

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
        else if (curr == '+') fprintf(output, "%d %s ", 4, "+");
        else if (curr == '-') fprintf(output, "%d %s ", 5, "-");
        else if (curr == '*') fprintf(output, "%d %s ", 6, "*");
        else if (curr == '/') fprintf(output, "%d %s ", 7, "/");
        else if (curr == '=') fprintf(output, "%d %s ", 9, "=");
        else if (curr == '(') fprintf(output, "%d %s ", 15, "(");
        else if (curr == ')') fprintf(output, "%d %s ", 16, ")");
        else if (curr == ',') fprintf(output, "%d %s ", 17, ",");
        else if (curr == ';') fprintf(output, "%d %s ", 18, ";");
        else if (curr == '.') fprintf(output, "%d %s ", 19, ".");
        // Handle multiple letter tokens
        else if (curr == ':') {
            // Get next character
            char next;
            fscanf(input, "%c", &next);
            
            if (next == '=') {
                fprintf(output, "%d %s ", 20, ":=");
            } else {
                printf("[ERR] Invalid token (:%c) is given\nEnd the program\n", next);
                exit(1);
            }
        }
        else if (curr == '<' || curr == '>') {
            // Get next character
            char next;
            fscanf(input, "%c", &next);
            
            if      (curr == '<' && next == '>') fprintf(output, "%d %s ", 10, "<>");
            else if (curr == '<' && next == '=') fprintf(output, "%d %s ", 12, "<=");
            else if (curr == '>' && next == '=') fprintf(output, "%d %s ", 14, ">=");
            // Handle the case that next character is not '>', '='
            else {
                fprintf(output, "%d %s ", curr == '<' ? 11 : 13, curr);
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
            
            fprintf(output, "%d %s ", 3, number);
        }
        // Handle reserved words or identifier
        else if (isLetter(curr)) {
            // Move cursor one back to get token.
            fseek(input, -1, SEEK_CUR);
            
            char* token = readUptoNonLetterNonDigit(input);
            printf("token: %s\n", token);
            
            if            (strcmp(token, "odd") == 0) fprintf(output, "%d %s ", 8, token);
            else if     (strcmp(token, "begin") == 0) fprintf(output, "%d %s ", 21, token);
            else if       (strcmp(token, "end") == 0) fprintf(output, "%d %s ", 22, token);
            else if        (strcmp(token, "if") == 0) fprintf(output, "%d %s ", 23, token);
            else if      (strcmp(token, "then") == 0) fprintf(output, "%d %s ", 24, token);
            else if     (strcmp(token, "while") == 0) fprintf(output, "%d %s ", 25, token);
            else if        (strcmp(token, "do") == 0) fprintf(output, "%d %s ", 26, token);
            else if      (strcmp(token, "call") == 0) fprintf(output, "%d %s ", 27, token);
            else if     (strcmp(token, "const") == 0) fprintf(output, "%d %s ", 28, token);
            else if       (strcmp(token, "var") == 0) fprintf(output, "%d %s ", 29, token);
            else if (strcmp(token, "procedure") == 0) fprintf(output, "%d %s ", 30, token);
            else if     (strcmp(token, "write") == 0) fprintf(output, "%d %s ", 31, token);
            else if      (strcmp(token, "read") == 0) fprintf(output, "%d %s ", 32, token);
            else if      (strcmp(token, "else") == 0) fprintf(output, "%d %s ", 33, token);
            // If token is not one of reserved word, treat it as identifier
            else {
                if (!isValidIdent(token)) {
                    printf("[ERR] Invalid ident (%s) is given\nEnd the program\n", token);
                    exit(1);
                }
                fprintf(output, "%d %s ", 2, token);
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
