[Project link](https://bitbucket.org/schneider128k/2016_fall_2016_cop3402/wiki/Project_0_PM0_Virtual_Machine)

## Brief description:
* reads in pm0 code,
* prints the assembler version of the pm0 code, and
* executes the pm0 code displaying the machine state after each instruction.

The file `mcode.pm0` contains an example program.
The file `trace.txt` is what your program should output when you run it on `mcode.pm0`.
**Note that stack[0] is not shown.** It is important that you print out | to separate the activation records.
The files `trace.txt` and `mcode.pm0` are located in the code repo under `Sources` in the folder [`0_virtual_machine`](https://bitbucket.org/schneider128k/2016_fall_2016_cop3402/src/867f7bb8fd3e268abc479dfeedd5b018615aafb1/0_virtual_machine/a_project_supporting_files/?at=master). There are more test cases in the subfolder test-cases.
## Assignment instructions and guidelines:
* the VM must be written in C and must compile/run on eustis2
* submit to Webcourses a single zip file containing all source codes files and the text file collaboration.txt describing the contributions of each team member
* if your source code consists of a single file, then call this file vm.c (we will compile your code with gcc vm.c)
* if your source code consists of multiple files, then you must also create a makefile and include it inside the zip file (without the makefile we would not know how to compile your code)
Both files should start with comments identifying all team members
```
// Team name:
// Name of first team member
// Name of second team member 
// ...`
```
## Compile and run the code guidelines:
Compile:
`gcc vm.c`
Run:
`./a.out mcode.pm0`
if you don't want to write the trace of the execution to the screen, use
`./a.out mcode.pm0 > trace.txt`
(or any other file name of your choice)
If you're wondering what > means, read about Unix pipes: https://en.wikipedia.org/wiki/Pipeline_(Unix) There are also < and | but we don't need them for this first project.
## Detailed description of project
### The P-machine:
Your task is to implement a virtual machine (VM) known as the P-machine (PM/0).
### Stack and code
The P-machine is a stack-based machine with two memory stores:
* stack that is organized as a stack and contains the data to be used by the PM/0 CPU
* code that is organized as a list and contains the instructions for the VM
Registers
The PM/0 CPU has four registers. The registers are:
bp base pointer
sp stack pointer
pc program counter
ir instruction register
Instruction format
The Instruction Set Architecture (ISA) of the PM/0 has 24 instructions. Each instruction consists of three components (non-negative integers) OP L M
that are separated by one space. The names of the components are:
OP operation code (op or opcode)
L lexicographical level (level)
M modifier
The modifier means depending on the opcode (mnemonic)
Meaning Instructions
number  LIT, INC
program address JMP, JPC, CAL
data address    LOD, STO
identity of the operator    OPR, SIO
The complete list of instructions is in Appendix A and B.
Struct representing instructions
It is useful to use the following struct to represent instructions:
struct {
int op;   /* opcode 
int  l;   /* L            
int  m;   /* M         
} instruction;
P-Machine Cycles:
The PM/0 instruction cycle is carried out in two steps:
Fetch Cycle
Execute Cycle
Fetch Cycle:
In the Fetch Cycle, an instruction is fetched from code store and placed in ir:
ir = code[pc];
The program counter is incremented by 1 to point to the next instruction to be executed:
pc = pc + 1;
Execute Cycle:
In the Execute Cycle, the instruction that was fetched is executed by the VM. The operation code that is stored in the field ir.op indicates the operation to be executed.
When the opcode ir.op equals 02 OPR or 09 SIO, then the modifier ir.m further identifies the instruction.
PM/0 Initial/Default Values:
Initial values for PM/0 CPU registers:
sp = 0;
bp = 1;
pc = 0;
ir = 0;
Initial stack store values: We just show the first three stack locations:
stack[1] = 0;
stack[2] = 0;
stack[3] = 0;
Constant Values:
MAX_STACK_HEIGHT = 2000;
MAX_CODE_LENGTH = 500;
MAX_LEXI_LEVELS = 3;
Appendix A - Instruction Set Architecture (ISA):
Instruction Description
01 LIT 0 M  Push value M onto stack
02 OPR 0 M  Perform arithmetic or logical operations defined in detail below)
03 LOD L M  Get value at offset M in frame L levels down and push it
04 STO L M  Pop stack and insert value at offset M in frame L levels down
05 CAL L M  Call procedure at M (generates new stack frame)
06 INC 0 M  Allocate M locals on stack
07 JMP 0 M  Jump to M
08 JPC 0 M  Pop stack and jump to M if value is equal to 0
09 SIO 0 0  OUT Pop stack and print out value
09 SIO 0 1  INP Read in input from user and push it
09 SIO 0 2  HLT Halt the machine
Appendix B - ISA Pseudo Code:
Instructions
01  LIT  0  M
sp = sp + 1;
stack[sp] = M;

02 OPR 0 M (defined below)

03  LOD  L  M
sp = sp + 1;
stack[sp] = stack[ base(L, bp) + M];

04  STO  L  M
stack[ base(L, bp) + M] = stack[ sp ];
sp = sp - 1;

05  CAL  L  M
stack[sp + 1] = 0;           /* return value (FV)
stack[sp + 2] = base(L, bp); /* static link (SL)
stack[sp + 3] = bp;          /* dynamic link (DL)
stack[sp + 4] = pc;          /* return address (RA)
bp = sp + 1;
pc = M;

06  INC  0  M
sp = sp + M;

07  JMP  0  M
pc = M;

08  JPC  0  M
if ( stack[ sp ] == 0 ) then { pc = M; }
sp = sp - 1;

SIO 0 M (defined below)
Arithmetic/logical instructions
02  OPR  0  M


0 RET  
sp = bp – 1; 
pc = stack[sp + 4]; 
bp = stack[sp + 3];

1 NEG  
stack[sp] = -stack[sp];

2 ADD  
sp = sp – 1; 
stack[sp] = stack[sp] + stack[sp + 1];

3 SUB  
sp = sp – 1; 
stack[sp] = stack[sp] - stack[sp + 1];

4 MUL  
sp = sp – 1; 
stack[sp] = stack[sp] * stack[sp + 1];

5 DIV  
sp = sp – 1; 
stack[sp] = stack[sp] / stack[sp + 1];

6 ODD  
stack[sp] = stack[sp] mod 2;

7 MOD  
sp = sp – 1; 
stack[sp] = stack[sp] mod stack[sp + 1];

8 EQL  
sp = sp – 1; 
stack[sp] = stack[sp] == stack[sp + 1];

9 NEQ  
sp = sp – 1; 
stack[sp] = stack[sp] != stack[sp + 1];

10 LSS  
sp = sp – 1; 
stack[sp] = stack[sp] <  stack[sp + 1];

11 LEQ  
sp = sp – 1; 
stack[sp] = stack[sp] <= stack[sp + 1];

12 GTR  
sp = sp – 1; 
stack[sp] = stack[sp] >  stack[sp + 1];

13 GEQ  
sp = sp – 1; 
stack[sp] = stack[sp] >= stack[sp + 1];
The result of a logical operation such as EQL or LSS is defined as 1 if the condition is true and 0 otherwise.
Input, output and halt instructions
09  SIO  0  0           
OUT
print(stack[ sp ]);
sp = sp – 1;

09  SIO  0  1           
INP
sp = sp + 1;
read(stack[ sp ]);

09  SIO  0  2           
HLT
halt;
