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
void printAssemblyCode();
char* getAssemblerCode(struct instruction instr);
char* getOpPart(struct instruction instr);
char* getLPart(char* opPart, struct instruction instr);
char* getMPart(char* opPart, struct instruction instr);



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
    
    printAssemblyCode();
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
    // Read instruction till the end of the file. If the number of instruction goes over MAX_CODE_LENGTH, don't store the rest of instructions.
    while (fscanf(input, "%d %d %d\n", &op, &l, &m) != EOF && numInstructions < MAX_CODE_LENGTH) {
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

void printAssemblyCode() {
    printf("PL/0 code:\n\n");
    
    int i;
    for (i = 0; i < numInstructions; i++) {
        printf("%3d  %s\n", i, getAssemblerCode(instructions[i]));
    }
}

char* getAssemblerCode(struct instruction instr) {
    char* opPart = getOpPart(instr);
    char* lPart  = getLPart(opPart, instr);
    char* mPart  = getMPart(opPart, instr);
    
    char* returnS = malloc(sizeof(char) * (strlen(opPart) + strlen(lPart) + strlen(mPart) + 2));
    sprintf(returnS, "%s%s%s  ", opPart, lPart, mPart);
    return returnS;
}

// With given instruction, return 3 assembly action code.
char* getOpPart(struct instruction instr) {
    int op = instr.op;
    int m = instr.m;
    
    if      (op == 1) return "LIT";
    else if (op == 2) {
        if      (m == 0) return "RET";
        else if (m == 1) return "NEG";
        else if (m == 2) return "ADD";
        else if (m == 3) return "SUB";
        else if (m == 4) return "MUL";
        else if (m == 5) return "DIV";
        else if (m == 6) return "ODD";
        else if (m == 7) return "MOD";
        else if (m == 8) return "EQL";
        else if (m == 9) return "NEQ";
        else if (m == 10) return "LSS";
        else if (m == 11) return "LEQ";
        else if (m == 12) return "GTR";
        else if (m == 13) return "GEQ";
    }
    else if (op == 3) return "LOD";
    else if (op == 4) return "STO";
    else if (op == 5) return "CAL";
    else if (op == 6) return "INC";
    else if (op == 7) return "JMP";
    else if (op == 8) return "JPC";
    else if (op == 9) {
        if      (m == 0) return "OUT";
        else if (m == 1) return "INP";
        else if (m == 2) return "HLT";
    }

    // It should never reaches here. When it reaches here, something weird is going on
    printf("Something wrong is happening.\n");
    return "";
}

char* getLPart(char* opPart, struct instruction instr) {
    // Handle opPart is not defined
    if (strcmp(opPart, "") == 0) return "";
    
    int i;
    int len = 3;
    char* showL[] = {"LOD", "STO", "CAL"};
    
    for (i = 0; i < len; i++) {
        if (strcmp(opPart, showL[i]) == 0) {
            char* lPart = malloc(sizeof(char) * 5);
            sprintf(lPart, "%5d", instr.l);
            return lPart;
        }
    }
    
    return "     ";
}

char* getMPart(char* opPart, struct instruction instr) {
    // Handle opPart is not defined
    if (strcmp(opPart, "") == 0) return "";

    int i;
    int len = 2;
    char* hideM[] = {"RET", "HLT"};
    
    for (i = 0; i < len; i++) {
        if (strcmp(opPart, hideM[i]) == 0) {
            return "     ";
        }
    }
    
    char* mPart = malloc(sizeof(char) * 5);
    sprintf(mPart, "%5d", instr.m);
    return mPart;
}









