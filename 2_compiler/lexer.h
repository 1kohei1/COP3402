#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUMBER_LIMIT 65535
#define IDENT_LENGTH_LIMIT 12
#define FILE_NAME_WITHOUT_COMMENT "source_without_comment.pl0"
#define TOKEN_LIST_FILE "token_list.txt"

typedef enum token_type {
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

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
