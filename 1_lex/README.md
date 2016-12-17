[Project link](https://bitbucket.org/schneider128k/2016_fall_2016_cop3402/wiki/Project_1_PL0_Lexer)
## Brief description
Your task is to implement a lexical analyzer (scanner) for the programming language PL/0 in C. You are not allowed to use any tools such as lex. Your program should be able to identify some errors and produce as output:

* the source program (without comments)
* the table of the recognized lexical tokens with token types and semantic values

An example of pl0 source code with corresponding lexer output is in Source in the folder [`1_lexer`](https://bitbucket.org/schneider128k/2016_fall_2016_cop3402/src/867f7bb8fd3e268abc479dfeedd5b018615aafb1/1_lexer/?at=master).
## Token types
```
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19,      
becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, 
constsym = 28, varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33
```
The token types are defined in the file `tokens.h` in source in the folder [`1_lexer`](https://bitbucket.org/schneider128k/2016_fall_2016_cop3402/src/867f7bb8fd3e268abc479dfeedd5b018615aafb1/1_lexer/?at=master).

## Identifiers and numbers
```
digit = [0-9]
letter = [a-zA-Z]
ident = letter (letter | digit)
number = (digit)+
```
The identifiers should be at most 12 characters long. The numbers should not be less or equal to 2^16 - 1.

## Reserved words
```
const, var, procedure, call, begin, end, if, then, else, while, do, read, write, odd
```
## Operators and special symbols
```
+ - * /           arithmetic operators
= <> <= < >= >=   comparison operators (note that = is also used to assign the value to a constant)
:=                assignment 
, ; .
( )
```
## Invisible characters
```
tab, white space, newline
```
## Comments
```
/* this is a comment */
```
## Constraints
The input should be specified as a cmd line arg. The output should be written to stdout. The source code should be output only if the flag `--source` is specified as a cmd line arg. The clean source code (without comments) should be output only if the flag `--clean` is specified as a cmd line arg.
## Lexical errors
* identifier does not start with a letter (for instance, 3foobar)
* identifier too long
* number greater than 2^16-1
* invalid token

Submission instructions
The file containing the main method should be called `lexer.c`. If your code consists of multiple files, you have to submit a makefile. Describe in detail the contributions of each team member in the text file `collaboration.txt`. Submit a single zip file containing all source code files and `collaboration.txt`. Your code must compile and run on eustis2.
