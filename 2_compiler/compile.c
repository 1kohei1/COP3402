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

typedef struct token_template {
    int tokenVal;
    char name[12];
} token_template;

// Global variables
int lexical_level = 0;
int numSymbol = 0;
struct token_template token;
FILE* tokenList;

// Compiler part
void compile();
void program();
void block();
void statement();

// Utility part
void setTokenList();
void closeTokenList();
void getNextToken();
void error(int err);
void printToken();
void printSymbol();
struct symbol* get_symbol(char name[12]);
void put_symbol(int kind, char name[12], int val, int level, int addr);



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
    emptySourceWithoutCommentFile();
    
    // Remove comments from input file so that we can handle print source without comment arg.
    removeCommentFromInputFile(inputFileName);
    
    // Do lexical analysis and output result to TOKEN_LIST
    lexicalAnalysis();

    compile();

    printSymbol();
    
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
    getNextToken();
    block();
    if (token.tokenVal != periodsym) {
        // error(9);
    }
}

void block() {
    // Handle const 
    if (token.tokenVal == constsym) {
        do {
            // Identifier
            getNextToken();
            if (token.tokenVal != identsym) {
                error(4);
            } 
            // Keep identifier name
            char name[12];
            strcpy(name, token.name);
            // Equal
            getNextToken();
            if (token.tokenVal != eqsym) {
                error(3);
            }
            // Number
            getNextToken();
            if (token.tokenVal != numbersym) {
                error(30);
            }
            int val = convertToInt(token.name);
            
            // Insert to symbol table
            put_symbol(1, name, val, 0, 0);
            
            // Get next token
            getNextToken();
        } while (token.tokenVal == commasym);
        if (token.tokenVal != semicolonsym) {
            error(5);
        }
        getNextToken();
    }
    // Handle var
    if (token.tokenVal == varsym) {
        do {
            getNextToken();
            if (token.tokenVal != identsym) {
                error(4);
            }
            
            // Insert to symbol table
            put_symbol(2, token.name, 0, lexical_level, 0); // Not sure about modifier here.
            
            // Get next token
            getNextToken();
        } while (token.tokenVal == commasym);
        if (token.tokenVal != semicolonsym) {
            error(5);
        }
        getNextToken();
    }
    // Handle procedure
    while (token.tokenVal == procsym) {
        getNextToken();
        if (token.tokenVal != identsym) {
            error(4);
        }
        getNextToken();
        if (token.tokenVal != semicolonsym) {
            error(5);
        }
        getNextToken();
        block();
        if (token.tokenVal != semicolonsym) {
            error(5);
        }
        getNextToken();
    }
    
    statement();
}

void statement() {
    
}


// Utility part
void setTokenList() {
    tokenList = openFile(TOKEN_LIST_FILE, "r");
}

void closeTokenList() {
    fclose(tokenList);
}

void getNextToken() {
    fscanf(tokenList, "%s %d ", token.name, &token.tokenVal);
    // printToken();
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
    else if (err == 30) printf("Number is expected.");
    else if (err == 31) printf("Reached the maximum number of symbol table.");

    printf("\n");

    exit(1);
}

void printToken() {
    printf("tokenVal: %d, name: %s\n", token.tokenVal, token.name);
}

void printSymbol() {
    int i;
    for (i = 0; i < numSymbol; i++) {
        struct symbol symbol = symbol_table[i];
        printf("kind: %d, name: %s, val: %d, level: %d, addr: %d\n", symbol.kind, symbol.name, symbol.val, symbol.level, symbol.addr);
    }
}

// function get_symbol that looks up a symbol in symbol table by name and
// returns pointer symbol if found and NULL if not found
struct symbol* get_symbol(char name[12]) {
    int i;
    for (i = 0; i < numSymbol; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return &symbol_table[i];
        }
    }
    return NULL;
}

// function put_symbol that puts a symbol into symbol table provided that 
// a symbol with this name does not exist. (calls error function if name already exists)
void put_symbol(int kind, char name[12], int val, int level, int addr) {
    if (get_symbol(name) != NULL) {
        // const
        if (kind == 1) {
            error(29);
        } else {
            error(28);
        }
    }
    
    if (numSymbol + 1 == MAX_SYMBOL_TABLE_SIZE) {
        error(31);
    }
    
    struct symbol symbol;
    symbol.kind = kind;
    strcpy(symbol.name, name);
    symbol.val = val;
    symbol.level = level;
    symbol.addr = addr;
    
    symbol_table[numSymbol] = symbol;
    numSymbol++;
}