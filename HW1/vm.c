/*
    Arturo Lara
    POS3402 - Summer
    Homework 1: P-Machine
    05/30/2025
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_CODE_LENGTH 500 // Maximum number of instructions that can be be loaded into memory
#define INITIAL_BP (MAX_CODE_LENGTH - 1) // Initial Activation Record Base Pointer

// Representation of a single instruction line from the input text file
typedef struct {
    // Opcode
    int op;
    // Lexicographical Level 
    int l;
    // Modifier
    int m; // Always 0 except for opcodes 3-5
} instruction;

// Stack
int pas[MAX_CODE_LENGTH] = {0};
// Program Counter 
int pc = 10;
// Base Pointer
int bp = INITIAL_BP;
// Stack Pointer
int sp = MAX_CODE_LENGTH;
// Halt flag
int halt = 1;
// Instruction Register
instruction ir;

// Find base L levels down
int base(int l, int bp) {
    int arb = bp;
    while (l > 0) {
        arb = pas[arb];
        l--;
    }
    return arb;
}

// Prints the information from the code execution
void printInfo() {
    // Print the instruction information out
    if (ir.op==2 && ir.m != 0)
        printf("%s\t%d\t%d\t%d\t%d\t%d\t",
            ir.m == 1 ? "ADD" : ir.m == 2 ? "SUB" : ir.m == 3 ? "MUL" :
            ir.m == 4 ? "DIV" : ir.m == 5 ? "EQL" : ir.m == 6 ? "NEQ" :
            ir.m == 7 ? "LSS" : ir.m == 8 ? "LEQ" : ir.m == 9 ? "GTR" :
            ir.m == 10 ? "GEQ" : "ERR", ir.l, ir.m, pc, bp, sp);
    else
        printf("%s\t%d\t%d\t%d\t%d\t%d\t", 
            (ir.op == 1 ? "INC" : ir.op == 2 ? "RTN" : ir.op == 3 ? "LOD" :
            ir.op == 4 ? "STO" : ir.op == 5 ? "CAL" : ir.op == 6 ? "LIT" :
            ir.op == 7 ? "JMP" : ir.op == 8 ? "JPC" : ir.op == 9 ? "SYS" : 
            "ERR"), ir.l, ir.m, pc, bp, sp);

    int dynBPs[MAX_CODE_LENGTH];
    int count = 0, cur = bp;

    // Build dynamic link chain
    while (1) {
        dynBPs[count++] = cur;
        if (cur == INITIAL_BP) 
            break;
        cur = pas[cur - 1];
    }

    // Print stack and separator mark for activation records
    for (int i = INITIAL_BP; i >= sp; i--) {
        for (int j = 0; j < count; j++) {
            if (i == dynBPs[j]) {
                printf("| ");
                break;
            }
        }
        printf("%d ", pas[i]);
    }
    printf("\n");
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
    while (fscanf(fp, "%d %d %d", &pas[10 + count], &pas[11 + count], &pas[12 + count]) == 3)
        count += 3;
    fclose(fp);

    // Print out initial values 
    printf("Initial values:\t%d\t%d\t%d\n", pc, bp, sp);

    // Loop to run commands and print them out
    while (halt) {
        // Fetch the current instruction
        ir.op = pas[pc];
        ir.l = pas[pc + 1];
        ir.m = pas[pc + 2];
        pc += 3;

        // Execute loop
        switch (ir.op) {
            // INC: Increment stack pointer by M spaces
            case 1:
                sp -= ir.m;
                break;
            // RTN / OPR: Arithmetic operations and returns
            case 2:
                switch (ir.m) {
                    case 0: // RTN
                        sp = bp + 1;
                        bp = pas[sp - 2];  // Return Address
                        pc = pas[sp - 3];  // Dynamic Link
                        break;
                    case 1: // ADD
                        pas[sp + 1] += pas[sp]; 
                        sp++;
                        break;
                    case 2: // SUB
                        pas[sp + 1] -= pas[sp]; 
                        sp++;
                        break;
                    case 3: // MUL
                        pas[sp + 1] *= pas[sp]; 
                        sp++;
                        break;
                    case 4: // DIV
                        pas[sp + 1] /= pas[sp]; 
                        sp++;
                        break;
                    case 5: // EQL
                        pas[sp + 1] = (pas[sp + 1] == pas[sp]); 
                        sp++;
                        break;
                    case 6: // NEQ
                        pas[sp + 1] = (pas[sp + 1] != pas[sp]); 
                        sp++;
                        break;
                    case 7: // LSS
                        pas[sp + 1] = (pas[sp + 1] < pas[sp]); 
                        sp++;
                        break;
                    case 8: // LEQ
                        pas[sp + 1] = (pas[sp + 1] <= pas[sp]); 
                        sp++;
                        break;
                    case 9: // GTR
                        pas[sp + 1] = (pas[sp + 1] > pas[sp]); 
                        sp++;
                        break;
                    case 10: // GEQ
                        pas[sp + 1] = (pas[sp + 1] >= pas[sp]); 
                        sp++;
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
                pas[sp - 1] = base(ir.l, bp); // Static link
                pas[sp - 2] = bp;             // Dynamic link
                pas[sp - 3] = pc;             // Return address
                bp = sp - 1;
                pc = ir.m;
                break;
            // LIT: Push literal constant M onto the stack
            case 6:
                sp--;
                pas[sp] = ir.m;
                break;
            // JMP: Jump to address M
            case 7:
                pc = ir.m;
                break;
            // JPC: Jump conditionally if top of stack is 0
            case 8:
                if (pas[sp] == 0)
                    pc = ir.m;
                sp++;
                break;
            // SYS: System calls
            case 9:
                switch (ir.m) {
                    case 1:
                        // Output value in stack[sp] and pop
                        printf("Output result is: %d\n", pas[sp]);
                        sp++;
                        break;
                    case 2:
                        // Read input integer and store on top of stack
                        printf("Please input a value: ");
                        sp--;
                        scanf("%d", &pas[sp]);
                        break;
                    case 3:
                        // Halt program
                        halt = 0;
                        break;
                }
                break;
        }
        printInfo();
    }
    return 0;
}
