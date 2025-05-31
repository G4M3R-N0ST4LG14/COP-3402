/*
    Arturo Lara
    POS3402 - Summer
    Homework 1: P-Machine
    05/30/2025
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000 // Maximum number of values that can be stored in the stack
#define MAX_CODE_LENGTH 500 // Maximum number of instructions that can be be loaded into memory
#define MAX_LEXI_LEVELS 3 // Maximum number of levels of nesting depth

// Representation of a single instruction line from the input text file
typedef struct {
    // Opcode
    int op;
    // Lexicographical Level 
    int l;
    // Modifier
    int m; // Always 0 except for opcodes 3-5
} instruction;

// Instruction Memory Array
instruction code[MAX_CODE_LENGTH];
// Stack
int pas[MAX_STACK_HEIGHT];
// Program Counter 
int pc = 0;
// Base Pointer
int bp = 499;
// Stack Pointer
int sp = 500;
// Instruction Register
instruction ir;

// Find base L levels down
int base(int l, int base) {
    while (l > 0) {
        base = pas[base];
        l--;
    }
    return base;
}

// Prints the information of the stack and appendix record past the "|" symbol
void printStack(int sp, int bp) {
    int display[6] = {0};
    int i, count = 0;

    // Fill display[] with the top 6 stack values, or 0 if not enough
    for (i = sp; i > sp - 6 && i >= 0; i--) {
        display[count++] = pas[i];
    }

    // Print values from right to left
    for (i = 5; i >= 0; i--) {
        printf("%d ", display[i]);
    }

    // Print marker bar
    printf("|");
}


int main(int argc, char *argv[]) {
    // Used to check for proper usage of command to run program
    // Proper Use: ./vm input.txt
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    
    // Opens input file
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Cannot open input file.\n");
        return 1;
    }

    // Reads each instruction from file
    int count = 0;
    // Inputs all data into code array and into each respective integer
    while (fscanf(fp, "%d %d %d", &code[count].op, &code[count].l, &code[count].m) != EOF) {
        count++;
    }
    fclose(fp);
    // Print out values 
    printf("Initial values:\t%d\t%d\t%d\n", pc, bp, sp);

    // Loop to run commands and print them out
    while (pc < count) {
        // Fetch the current instruction
        ir = code[pc];
        pc++;
        
        // Decodes the current instruction
        switch (ir.op) {
            // INC: Increment stack pointer by M spaces
            case 1:
                sp -= ir.m;
                break;
            // RTN: Arithmetic operations and ends AR
            case 2: 
                switch (ir.m) {
                    // RTN
                    case 0:
                        sp = bp + 1;
                        pc = pas[bp - 2];  // Return Address
                        bp = pas[bp - 1];  // Dynamic Link
                        break;
                    // ADD
                    case 1:
                        sp++;
                        pas[sp] = pas[sp] + pas[sp - 1];
                        break;
                    // SUB
                    case 2:
                        sp++;
                        pas[sp] = pas[sp] - pas[sp - 1];
                        break;
                    // MUL
                    case 3:
                        sp++;
                        pas[sp] = pas[sp] * pas[sp - 1];
                        break;
                    // DIV
                    case 4: 
                        sp++;
                        pas[sp] = pas[sp] / pas[sp - 1];
                        break;
                    // EQL
                    case 5:
                        sp++;
                        pas[sp] = (pas[sp] == pas[sp - 1]);
                        break;
                    // NEQ
                    case 6:
                        sp++;
                        pas[sp] = (pas[sp] != pas[sp - 1]);
                        break;
                    // LSS
                    case 7:
                        sp++;
                        pas[sp] = (pas[sp] < pas[sp - 1]);
                        break;
                    // LEQ
                    case 8:
                        sp++;
                        pas[sp] = (pas[sp] <= pas[sp - 1]);
                        break;
                    // GTR
                    case 9:
                        sp++;
                        pas[sp] = (pas[sp] > pas[sp - 1]);
                        break;
                    // GEQ
                    case 10:
                        sp++;
                        pas[sp] = (pas[sp] >= pas[sp - 1]);
                        break;
                    default:
                        printf("Invalid RTN operation code: %d\n", ir.m);
                        break;
                }
                break;
            // LOD: Load value from L levels down and M offset
            case 3:
                sp--;
                pas[sp] = pas[base(ir.l, bp) - ir.m];
                break;
            // STO: Store value from top of stack to L levels down at M offset
            case 4:
                pas[base(ir.l, bp) - ir.m] = pas[sp];
                sp++;
                break;
            // CAL: Procedure call at code address M
            case 5:
                // Static link
                pas[sp - 1] = base(ir.l, bp);
                // Dynamic link
                pas[sp - 2] = bp;
                // Return address
                pas[sp - 3] = pc;
                bp = sp - 1;
                pc = ir.m;
                break;
            // LIT: Push literal constant M in top of stack
            case 6:
                sp--;
                pas[sp] = ir.m;
                break;
            // JMP: Jump to address M
            case 7: 
                pc = ir.m;
                break;
            // JPC: Jump conditionally if value of stack[sp] is 0 then jump to M and pop stack
            case 8:
                if (pas[sp] == 0)
                    pc = ir.m;
                sp++;
                break;
            // SYS: 
            case 9:
                // Output value in stack[sp] and pop
                if (ir.m == 1) {
                    printf("Output result is: %d\n", pas[sp]);
                    sp++;
                } 
                // Read input integer and store on top of stack
                else if (ir.m == 2) {
                    printf("Please input a value: ");
                    sp--;
                    scanf("%d", &pas[sp]);
                } 
                // Halt program
                else if (ir.m == 3) {
                    exit(0);
                }
                break;
            default:
                printf("Unhandled OP code: %d\n", ir.op);
                break;
        }

        // Prints out all information related to the instruction
        if (ir.op == 9 && ir.m == 1)
            printf("SYS\t%d\t%d\t%d\t%d\t%d\t", ir.l, ir.m, pc, bp, sp);
        else if (ir.op==2 && ir.m != 0){
            printf("%s\t%d\t%d\t%d\t%d\t%d\t",
                 ir.m == 1 ? "ADD" : ir.m == 2 ? "SUB" : ir.m == 3 ? "MUL" :
                 ir.m == 4 ? "DIV" : ir.m == 5 ? "EQL" : ir.m == 6 ? "NEQ" :
                 ir.m == 7 ? "LSS" : ir.m == 8 ? "LEQ" : ir.m == 9 ? "GTR" :
                 ir.m == 10 ? "GEQ" : "ERROR",
                 ir.l, ir.m, pc, bp, sp);
        }
        else
            printf("%s\t%d\t%d\t%d\t%d\t%d\t", 
                (ir.op == 1 ? "INC" : ir.op == 2 ? "RTN" : ir.op == 3 ? "LOD" :
                 ir.op == 4 ? "STO" : ir.op == 5 ? "CAL" : ir.op == 6 ? "LIT" :
                 ir.op == 7 ? "JMP" : ir.op == 8 ? "JPC" : ir.op == 9 ? "SYS" : "ERROR"),
                ir.l, ir.m, pc, bp, sp);

        // Print what's on the stack
        printStack(sp, bp);
        printf("\n");

    }

    return 0;
}
