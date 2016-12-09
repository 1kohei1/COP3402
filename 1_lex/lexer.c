#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INPUT_FILE "input.pl0"
#define FILE_NAME_WITHOUT_COMMENT "source_without_comment.pl0"

int shouldPrintSource = 0;
int shouldPrintSourceWithoutComment = 0;

void printFile(char* header, char* fileName);

int main(int argc, char** argv) {
    char inputFileName[100];
    // Get input file name. If it is executed without any arg, use default input file
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
    
    // Handle parameters
    if (shouldPrintSource) {
        printFile("source code:", inputFileName);
    }
    if (shouldPrintSourceWithoutComment) {
        printFile("source code without comments:", FILE_NAME_WITHOUT_COMMENT);
    }
    
}

void printFile(char* header, char* fileName) {
    // Opens the file. If file is not found, exit the program
    FILE* f;
    f = fopen(fileName, "r");
    if (f == NULL) {
        printf("[ERR] Failed to open file %s\n", fileName);
        exit(1);
    }
    
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




