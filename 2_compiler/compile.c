#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#define DEFAULT_INPUT_FILE "input.pl0"
#define DEFAULT_OUTPUT_FILE "output.txt"
#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
    int kind; // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
} symbol;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

typedef struct token {
    int tokenVal;
    struct symbol symbol;
} token;

// Global variables
int lexcal_level = 0;
FILE* tokenList;

// Compiler part
void compile();
void program();
void block(struct token token);

// Utility part
void setTokenList();
void closeTokenList();
struct token getToken();
void error(int err);



int main(int argc, char** argv) {
    char inputFileName[100];
    char outputFileName[100];
    
    // Get input file name and output file
    if (argc == 1) {
        strcpy(inputFileName, DEFAULT_INPUT_FILE);
        strcpy(outputFileName, DEFAULT_OUTPUT_FILE);
    } else {
        strcpy(inputFileName, argv[1]);
        strcpy(outputFileName, argv[2]);
    }

    // // Make source without comment file empty
    // emptySourceWithoutCommentFile();
    
    // Remove comments from input file so that we can handle print source without comment arg.
    // removeCommentFromInputFile(inputFileName);
    
    // Do lexical analysis and output result to TOKEN_LIST
    // lexicalAnalysis();



}

// Compiler part

void compile() {
    setTokenList();
    // Start compiling part
    program();
    // Close token list
    closeTokenList();
}

void program() {
    struct token token = getToken();
    program(token);
    if (token != periodsym) {
        error(9);
    }
}

void block(struct token token) {
    if (token == constsym) {
        token = getToken();
    }
    if (token == varsym) {
        
    }
    
}


// Utility part
void setTokenList() {
    // tokenList = openFile(TOKEN_LIST_FILE, "r");
}

void closeTokenList() {
    fclose(tokenList);
}

struct token getToken() {
    int tokenVal;
    char name[100];
    
    fscanf(tokenList, "%s %d ", name, &tokenVal);

    struct token token;
    token.tokenVal = tokenVal;

    return token;
}

/**
 * Print out error statement and end the program.
 * Some of errors are already caught in lexical analysis part.
 */ 
void error(int err) {
    if (err == 1) printf("Use = instead of :=.");
    else if (err == 2) printf("= must be followed by a number");
    else if (err == 3) printf("Identifier must be followed by =.");
    else if (err == 4) printf("const, var, procedure must be followed by identifier.");
    else if (err == 5) printf("Semicolon or comma missing.");
    else if (err == 6) printf("Incorrect symbol after procedure declaration.");
    else if (err == 7) printf("Statement expected.");
    else if (err == 8) printf("Incorrect symbol after statement part in block.");
    else if (err == 9) printf("Period expected.");
    else if (err == 10) printf("Semicolon between statements missing.");
    else if (err == 11) printf("Undeclared identifier.");
    else if (err == 12) printf("Assignment to constant or procedure is not allowed.");
    else if (err == 13) printf("Assignment operator expected.");
    else if (err == 14) printf("call must be followed by an identifier.");
    else if (err == 15) printf("Call of a constant or variable is meaningless.");
    else if (err == 16) printf("then expected.");
    else if (err == 17) printf("Semicolon or end expected.");
    else if (err == 18) printf("do expected.");
    else if (err == 19) printf("Incorrect symbol following statement.");
    else if (err == 20) printf("Relational operator expected.");
    else if (err == 21) printf("Expression must not contain a procedure identifier.");
    else if (err == 22) printf("Right parenthesis missing.");
    else if (err == 23) printf("The preceding factor cannot begin with this symbol.");
    else if (err == 24) printf("An expression cannot begin with this symbol.");
    else if (err == 25) printf("This number is too large.");
    else if (err == 26) printf("out must be followed by an expression.");
    else if (err == 27) printf("in must be followed by an identifier.");
    else if (err == 28) printf("Cannot reuse this symbol here.");
    else if (err == 29) printf("Cannot redefine constants.");

    printf("\n");

    exit(1);
}
