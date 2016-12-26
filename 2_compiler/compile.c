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

int lexcal_level = 0;

// Compiler part
void compile();
void procedure();

// Parser part


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
    program();
}

void program() {
    
}


// Utility part