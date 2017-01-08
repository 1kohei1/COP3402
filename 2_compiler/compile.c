#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#define DEFAULT_INPUT_FILE "input.pl0"
#define DEFAULT_OUTPUT_FILE "output.txt"
#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_LENGTH 500

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

struct pm0Code {
    int op;   /* opcode */
    int  l;   /* L */
    int  m;   /* M */
};

// Global variables
int lexical_level = 0;
int numSymbol = 0;
struct token_template token;
FILE* tokenList;
int codeIndex = 0;
struct pm0Code pm0Codes[MAX_CODE_LENGTH];

// Compiler part
void compile();
void program();
void block();
void constHandler();
int varHandler();
void procHandler();
void statement();
void condition();
int relation();
void expression();
void term();
void factor();

// Utility part
void setTokenList();
void closeTokenList();
void writePM0ToOutput(char outputFileName[100]);
void getNextToken();
void error(int err);
void printToken();
void printSymbol();
struct symbol* get_symbol(char name[12]);
void put_symbol(int kind, char name[12], int val, int level, int addr);
void insertPM0Code(int op, int l, int m);



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

    // If compile is executed successfully, write pm0 to output file.
    writePM0ToOutput(outputFileName);
    
    // When the program reaches here, it is syntactically correct.
    printf("No errors, program is syntactically correct.\n");
}

// Compiler part

void compile() {
    setTokenList();

    program();

    closeTokenList();
}

void program() {
    getNextToken();
    block();
    if (token.tokenVal != periodsym) {
        error(9);
    }
}

void block() {
    int numVar = 0;
    if (token.tokenVal == constsym) {
        constHandler();
    }
    if (token.tokenVal == varsym) {
        numVar = varHandler();
    }
    while (token.tokenVal == procsym) {
        procHandler();
    }
    
    // Allocate memory required for var
    insertPM0Code(6, 0, 4 + numVar);

    statement();
}

void constHandler() {
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
        
        // Insert const declaration to symbol table
        put_symbol(1, name, val, 0, 0);
        
        // Get next token
        getNextToken();
    } while (token.tokenVal == commasym);
    if (token.tokenVal != semicolonsym) {
        error(5);
    }
    getNextToken();
}

int varHandler() {
    int numVar = 0;
    do {
        getNextToken();
        if (token.tokenVal != identsym) {
            error(4);
        }
        
        // Insert to symbol table
        put_symbol(2, token.name, 0, lexical_level, 4 + numVar);
        
        // Get next token
        getNextToken();
        // Increment the number of var
        numVar++;
    } while (token.tokenVal == commasym);
    if (token.tokenVal != semicolonsym) {
        error(5);
    }
    getNextToken();
    
    return numVar;
}

void procHandler() {
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

void statement() {
    if (token.tokenVal == identsym) {
        // Check this ident exists in symbol table
        struct symbol* symbol = get_symbol(token.name);
        if (symbol == NULL) {
            error(11);
        }
        getNextToken();
        if (token.tokenVal != becomessym) {
            error(3);
        }
        getNextToken();
        expression();

        // Set the result of computation back to the location of the variable
        insertPM0Code(4, symbol->level, symbol->addr);
    } else if (token.tokenVal == callsym) {
        getNextToken();
        if (token.tokenVal != identsym) {
            error(14);
        }
        // Check this ident exists in symbol table
        if (get_symbol(token.name) == NULL) {
            error(11);
        }

        getNextToken();
    } else if (token.tokenVal == beginsym) {
        getNextToken();
        statement();
        while (token.tokenVal == semicolonsym) {
            getNextToken();
            statement();
        }
        if (token.tokenVal != endsym) {
            error(17);
        }
        getNextToken();
    } else if (token.tokenVal == ifsym) {
        getNextToken();
        condition();
        if (token.tokenVal != thensym) {
            error(16);
        }
        getNextToken();

        int tempCodeIndex = codeIndex;
        insertPM0Code(8, 0, 0);
        statement();
        pm0Codes[tempCodeIndex].m = codeIndex;
    } else if (token.tokenVal == whilesym) {
        int codeIndex1 = codeIndex;
        getNextToken();
        condition();
        int codeIndex2 = codeIndex;
        insertPM0Code(8, 0, 0);
        if (token.tokenVal != dosym) {
            error(18);
        }
        getNextToken();
        statement();
        insertPM0Code(7, 0, codeIndex1);
        pm0Codes[codeIndex2].m = codeIndex;
    } else if (token.tokenVal == readsym || token.tokenVal == writesym) {
        // Token gets updated for ident check. So save tokenVal at this point to differentiate read or write.
        int tokenVal = token.tokenVal;
        getNextToken();
        if (token.tokenVal != identsym) {
            error(33);
        }
        // Check this ident exists in symbol table
        struct symbol* symbol = get_symbol(token.name);
        if (symbol == NULL) {
            error(11);
        }

        if (tokenVal == readsym) {
            // Get value from standard input
            insertPM0Code(9, 0, 1);
            // Move that value to correct level and address
            insertPM0Code(4, symbol->level, symbol->addr);
        } else if (tokenVal == writesym) {
            // Writing const to STO
            if (symbol->kind == 1) {
                insertPM0Code(1, 0, symbol->val);
                insertPM0Code(9, 0, 0);
            }
            // Writing var to STO
            else {
                insertPM0Code(3, symbol->level, symbol->addr);
                insertPM0Code(9, 0, 0);
            }
        }

        getNextToken();
    }
    
}

void condition() {
    if (token.tokenVal == oddsym) {
        getNextToken();
        expression();
    } else {
        expression();
        if (!relation()) {
            error(20);
        }
        getNextToken();
        expression();
    }
}

int relation() {
    int val = token.tokenVal;
    
    return val == eqsym || val == neqsym || val == lessym || val == leqsym || val == gtrsym || val == geqsym;
}

void expression() {
    int tokenVal;
    if (token.tokenVal == plussym || token.tokenVal == minussym) {
        tokenVal = token.tokenVal;
        getNextToken();
        term();
        // Negate
        if (tokenVal == minussym) {
            insertPM0Code(2, 0, 1);
        }
    } else {
        term();
    }

    while (token.tokenVal == plussym || token.tokenVal == minussym) {
        tokenVal = token.tokenVal;
        getNextToken();
        term();
        // Addition
        if (tokenVal == plussym) {
            insertPM0Code(2, 0, 2);
        }
        // Subtraction
        else {
            insertPM0Code(2, 0, 3);
        }
    }
}

void term() {
    int tokenVal;
    factor();
    while (token.tokenVal == multsym || token.tokenVal == slashsym) {
        tokenVal = token.tokenVal;
        getNextToken();
        factor();
        // Multiplication
        if (tokenVal == multsym) {
            insertPM0Code(2, 0, 4);
        }
        // Division
        else {
            insertPM0Code(2, 0, 5);
        }
    }
}

void factor() {
    if (token.tokenVal == identsym) {
        // Check this ident exists in symbol table
        struct symbol* symbol = get_symbol(token.name);
        if (symbol == NULL) {
            error(11);
        }

        // Push that value to the top of the stack
        insertPM0Code(3, symbol->level, symbol->addr);

        getNextToken();
    } else if (token.tokenVal == numbersym) {
        // Get a number and push it at the top of the stack
        int val = convertToInt(token.name);
        insertPM0Code(1, 0, val);

        getNextToken();
    } else if (token.tokenVal == lparentsym) {
        getNextToken();
        expression();
        if (token.tokenVal != rparentsym) {
            error(22);
        }
        getNextToken();
    } else {
        error(32);
    }
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

void writePM0ToOutput(char outputFileName[100]) {
    FILE* outputFile = openFile(outputFileName, "w");

    int i;
    for (i = 0; i < codeIndex; i++) {
        fprintf(outputFile, "%d %d %d\n", pm0Codes[i].op, pm0Codes[i].l, pm0Codes[i].m);
    }

    fclose(outputFile);
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
    // Custom error
    else if (err == 30) printf("Number is expected.");
    else if (err == 31) printf("Reached the maximum number of symbol table.");
    else if (err == 32) printf("Factor couldn't be solved.");
    else if (err == 33) printf("Identifiler must follow read or write.");
    else if (err == 34) printf("Reached the maximum code the program can store");

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

void insertPM0Code(int op, int l, int m) {
    if (codeIndex + 1 == MAX_CODE_LENGTH) {
        error(34);
    }
    pm0Codes[codeIndex].op = op;
    pm0Codes[codeIndex].l = l;
    pm0Codes[codeIndex].m = m;

    codeIndex++;
}