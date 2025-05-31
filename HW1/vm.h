// In this homework you will form a team and implement a virtual machine called the P-machine.
// The P-machine is a stack machine that conceptually has one memory area called the process address space (PAS).
//----------------------------------------------------------------------------------------------------------------
// Student Name: David Tyler Jackson
// Student ID: 5298477
// Course: COP 3402 - Systems Software
// Instructor: Dr. Montagne

// This is the header file for the P-machine virtual machine.
// This file contains the declarations and definitions for the P-machine virtual machine.

#ifndef PMACHINE_H
#define PMACHINE_H

#define ARRAY_SIZE 500
#define INITIAL_BP (ARRAY_SIZE - 1)

// Structure to represent an instruction with OP, L, and M fields
typedef struct
{
    int op; // Opcode
    int l;  // Lexicographical Level
    int m;  // Modifier
} InstructionRegister;

const char *oprNames[] = {
    "RTN", // 0: Return
    "ADD", // 1: Addition
    "SUB", // 2: Subtraction
    "MUL", // 3: Multiplication
    "DIV", // 4: Divison
    "EQL", // 5: Equal
    "NEQ", // 6: Not Equal
    "LSS", // 7: Less than
    "LEQ", // 8: Less than or equal
    "GTR", // 9: Greater than
    "GEQ", // 10: Greater than or equal
};

const char *opCodeNames[] = {
    "INC", // 1 Increment
    "OPR", // 2 Return
    "LOD", // 3 Load to Stack
    "STO", // 4 Store
    "CAL", // 5 Call procedure
    "LIT", // 6 Push Literal
    "JMP", // 7 Jump to command
    "JPC", // 8 Jump conditionally
    "SYS", // 9 System calls
};

// Function declarations
int base(int L, int BP);
void printState();

#endif // PMACHINE_H
