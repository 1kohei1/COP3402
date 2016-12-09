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
int halt = 0;
int numAR = 0;                  // The number of activation record in VM. It doesn't count main AR
int ARBase[MAX_LEXI_LEVELS];    // This stores the base of each activation record.

// Registers
int sp = 0;
int bp = 1;
int pc = 0;
struct instruction ir;

// Functions for main functionalities
int readInstructions(char* inputFileName);
void printAssemblyCode();
void runVMandPrint();
// Functions for generating assembly code
char* getAssemblerCode(struct instruction instr);
char* getOpPart(struct instruction instr);
char* getLPart(char* opPart, struct instruction instr);
char* getMPart(char* opPart, struct instruction instr);
// Functions for running virtual machine
int base(int l, int b);
void executeInstruction();
void printVMStatus();
void printStack();
int shouldPrintPipe(int index);
// Utility functions for debug
void printInstructions();



int main(int argc, char** argv) {
    char inputFileName[100];
    // Get input file name
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
    
    // First print PM/0 code,
    printAssemblyCode();
    // Then, print VM status step by step.
    runVMandPrint();
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
    printf("\n");
}

char* getAssemblerCode(struct instruction instr) {
    char* opPart = getOpPart(instr);
    char* lPart  = getLPart(opPart, instr);
    char* mPart  = getMPart(opPart, instr);
    
    // With combinding all parts, generate one assembly PL/0 code.
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
    printf("Something wrong is happening. 1\n");
    return "";
}

char* getLPart(char* opPart, struct instruction instr) {
    // Handle opPart is not defined
    if (strcmp(opPart, "") == 0) return "";
    
    int i;
    int len = 3;
    char* showL[] = {"LOD", "STO", "CAL"};
    
    for (i = 0; i < len; i++) {
        // We need to show L in PL/0. Generate string with L in it.
        if (strcmp(opPart, showL[i]) == 0) {
            char* lPart = malloc(sizeof(char) * 5);
            sprintf(lPart, "%5d", instr.l);
            return lPart;
        }
    }
    
    // When the program reaches here, we don't need to show L. Just return 5 spaces.
    return "     ";
}

char* getMPart(char* opPart, struct instruction instr) {
    // Handle opPart is not defined
    if (strcmp(opPart, "") == 0) return "";

    int i;
    int len = 2;
    char* hideM[] = {"RET", "HLT"};
    
    for (i = 0; i < len; i++) {
        // If we don't need to show M value, return 5 spaces here.
        if (strcmp(opPart, hideM[i]) == 0) {
            return "     ";
        }
    }
    
    // When the program reaches here, we need to show M in PL/0. So generate string with M
    char* mPart = malloc(sizeof(char) * 5);
    sprintf(mPart, "%5d", instr.m);
    return mPart;
}

void runVMandPrint() {
    // Print VM header
    printf("Execution:\n");
    printf("                      pc   bp   sp   stack\n");
    printf("                     %3d  %3d  %3d   \n", pc, bp, sp);
    
    while (halt == 0) {
        // Fetch
        ir = instructions[pc];
        // Print current pc and assembly code
        char* assemblyCode = getAssemblerCode(ir);
        printf("%3d  %s", pc, assemblyCode);
        pc++;
        
        // Execute
        executeInstruction();
     
        // Print status of VM
        printVMStatus();   
    }
}

int base(int l, int b) {
    while (l > 0) {
        b = stack[b + 1];
        l--;
    }
    return b;
}

// Execute instruction stored in ir
void executeInstruction() {
    // Save them into another local variable to make code readable
    int op = ir.op;
    int l = ir.l;
    int m = ir.m;
    
    if (op == 1) {          // LIT
        sp++;
        stack[sp] = m;
    } else if (op == 2) {   // OPR
        if (m == 0) {           // RET
            sp = bp - 1;
            pc = stack[sp + 4];
            bp = stack[sp + 3];
            
            // Number of activation record changed. Decrease numAR by 1.
            numAR--;
            
        } else if (m == 1) {    // NEG
            stack[sp] = (-1) * stack[sp];
        } else if (m == 2) {    // ADD
            sp--;
            stack[sp] = stack[sp] + stack[sp + 1];
        } else if (m == 3) {    // SUB
            sp--;
            stack[sp] = stack[sp] - stack[sp + 1];
        } else if (m == 4) {    // MUL
            sp--;
            stack[sp] = stack[sp] * stack[sp + 1];
        } else if (m == 5) {    // DIV
            sp--;
            stack[sp] = stack[sp] / stack[sp + 1];
        } else if (m == 6) {    // ODD
            stack[sp] = stack[sp] % 2;
        } else if (m == 7) {    // MOD
            sp--;
            stack[sp] = stack[sp] % stack[sp + 1];
        } else if (m == 8) {    // EQL
            sp--;
            stack[sp] = stack[sp] == stack[sp + 1];
        } else if (m == 9) {    // NEQ
            sp--;
            stack[sp] = stack[sp] != stack[sp + 1];
        } else if (m == 10) {   // LSS
            sp--;
            stack[sp] = stack[sp] < stack[sp + 1];
        } else if (m == 11) {   // LEQ
            sp--;
            stack[sp] = stack[sp] <= stack[sp + 1];
        } else if (m == 12) {   // GTR
            sp--;
            stack[sp] = stack[sp] > stack[sp + 1];
        } else if (m == 13) {   // GEQ
            sp--;
            stack[sp] = stack[sp] >= stack[sp + 1];
        } else {
            // We should never reach here. When we reach here, something is wrong.
            printf("Something is wrong 2\n");
        }
    } else if (op == 3) {   // LOD
        sp++;
        stack[sp] = stack[base(l, bp) + m];
    } else if (op == 4) {   // STO
        stack[base(l, bp) + m] = stack[sp];
        sp--;
    } else if (op == 5) {   // CAL
        stack[sp + 1] = 0;
        stack[sp + 2] = base(l, bp);
        stack[sp + 3] = bp;
        stack[sp + 4] = pc;
        bp = sp + 1;
        pc = m;
        
        // New activation record is inserted. Update numAR and ARBase
        ARBase[numAR] = bp;
        numAR++;
        
    } else if (op == 6) {   // INC
        sp += m;
    } else if (op == 7) {   // JMP
        pc = sp + m;
    } else if (op == 8) {   // JPC
        if (stack[sp] == 0) {
            pc = m;
        }
        sp--;
    } else if (op == 9) {   // SIO
        if (m == 0) {           // OUT
            printf("%d\n", stack[sp]);
            sp--;
        } else if (m == 1) {    // IN
            sp++;
            scanf("%d", &stack[sp]);
        } else if (m == 2) {    // HLT
            halt = 1;
        } else {
        // We should never reach here. When we reach here, something is wrong.
        printf("Something is wrong 3\n");
        }
    } else {
        // We should never reach here. When we reach here, something is wrong.
        printf("Something is wrong 4\n");
    }
    
}

// Print VM status
void printVMStatus() {    
    // Print pc, bp, sp
    printf("%4d%5d%5d   ", pc, bp, sp);
    
    // Print stack
    printStack();
    
    // Move to new line
    printf("\n");
}

void printStack() {
    int i;
    for (i = 1; i <= sp; i++) {
        if (shouldPrintPipe(i)) {
            printf("| ");
        }
        printf("%d ", stack[i]);
    }
}

// Determine if given index is the start of activation record
int shouldPrintPipe(int index) {
    int i;
    for (i = 0; i < numAR; i++) {
        if (index == ARBase[i]) {
            return 1;
        }
    }
    
    return 0;
}


