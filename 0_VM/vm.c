#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INPUT_FILE "mcode.pm0"
#define DEFAULT_OUTPUT_FILE "trace.txt"
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

struct instruction {
    int op;   /* opcode */
    int  l;   /* L */
    int  m;   /* M */
};

// Machine variables
int stack[MAX_STACK_HEIGHT];
struct instruction instructions[MAX_CODE_LENGTH];

// Other helper variables
int numInstructions = 0;

// Registers
int sp = 0;
int bp = 1;
int pc = 0;
int ir = 0;

// Function declarations
int readInstructions(char* inputFileName);
void printInstructions();

int main(int argc, char** argv) {
    int i = 0;

    // Get input file name
    char inputFileName[100];
    if (argc == 2) {
        strcpy(inputFileName, argv[1]);
    } else {
        strcpy(inputFileName, DEFAULT_INPUT_FILE);
    }
    
    // Save instructions to instructions variable
    int shouldBreak = readInstructions(inputFileName);
    // Error happened. Stop the program
    if (shouldBreak) {
        return 0;
    }
    
}

// Save each instructions into instructions. 
// If error happens, return 1 and stop the main program. Otherwise, return 0.
int readInstructions(char* inputFileName) {
    FILE *input;
    input = fopen(inputFileName, "r");
    if (input == NULL) {
        printf("[ERR] Failed to open the file %s\n", inputFileName);
        return 1;
    }

    char* line;
    int op, l, m;
    while (fscanf(input, "%d %d %d\n", &op, &l, &m) != EOF) {
        struct instruction newInstruction;
        newInstruction.op = op;
        newInstruction.l = l;
        newInstruction.m = m;
        instructions[numInstructions] = newInstruction;
        numInstructions++;
    }
    
    // Close the file
    fclose(input);
    
    return 0;
}

// Helper function to print all instructions
void printInstructions() {
    int i;
    for (i = 0; i < numInstructions; i++) {
        struct instruction instr = instructions[i];
        printf("%d %d %d\n", instr.op, instr.l, instr.m);
    }
}



