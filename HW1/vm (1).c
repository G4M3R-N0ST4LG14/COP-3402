// In this homework you will form a team and implement a virtual machine called the P-machine.
// The P-machine is a stack machine that conceptually has one memory area called the process address space (PAS).
//----------------------------------------------------------------------------------------------------------------
// Student Name: David Tyler Jackson
// Student ID: 5298477
// Course: COP 3402 - Systems Software
// Instructor: Dr. Montagne

// This is the main file for the P-machine virtual machine.
// This file contains the main function and the implementation of the P-machine virtual machine.

#include <stdio.h>
#include <stdlib.h>
#include "../include/vm.h"

// Instruction Register Struct
// Initialization of Global Variables
int pas[ARRAY_SIZE] = {0};
int BPStack[ARRAY_SIZE] = {0};
int PC = 10, BP = 499, SP = 500, halt = 1;
InstructionRegister IR;

// DO NOT MODIFY THE FOLLOWING FUNCTION
int base(int L, int BP)
{
	int arb = BP;
	while (L > 0)
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

// Function to print the current state of the virtual machine
void printState()
{
	// print the instruction using the IR struct
	if (IR.op == 2)
		printf("%s %d  %d", oprNames[IR.m], IR.l, IR.m);
	else
		printf("%s %d  %d", opCodeNames[IR.op - 1], IR.l, IR.m);

	// print registers
	printf("\t%d\t%d\t%d\t", PC, BP, SP);

	// build dynamic‐link chain
	// This will hold the base pointers in the chain to link back and determine how many
	// bars is needed to print.
	int dynCount = 0;
	int dynBPs[ARRAY_SIZE];
	int cur = BP;
	while (1)
	{
		dynBPs[dynCount++] = cur;
		// Break once we reach the initial base pointer
		// Which is the initial activation record AKA: SP - 1
		if (cur == INITIAL_BP)
			break;
		// Dynamic Link is held at cur - 1
		// Dynamic Link points to the Activcation Record of the previous procedure
		cur = pas[cur - 1];
	}

	// print stack and separators
	for (int i = ARRAY_SIZE - 1; i >= SP; i--)
	{
		// if this index matches any BP in the chain, print '| '
		for (int j = 0; j < dynCount; j++)
		{
			if (i == dynBPs[j])
			{
				printf("| ");
				break;
			}
		}
		printf("%d ", pas[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{

	// Validate an input file was inserted.
	if (argc < 2)
	{
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	// Open file
	FILE *fp = fopen(argv[1], "r");
	if (!fp)
	{
		printf("Error: Cannot open file %s\n", argv[1]);
		return 1;
	}

	// Continually read integers from the file until EOF
	// loads integers into the text section of the pas array.
	int i = 0;
	while (fscanf(fp, "%d %d %d",
				  &pas[10 + i],
				  &pas[10 + i + 1],
				  &pas[10 + i + 2]) == 3)
		i += 3;

	fclose(fp);

	// Print initial state
	printf("\tPC\tBP\tSP\tStack\nInitial values:\t%d\t%d\t%d\n", PC, BP, SP);

	// Run the virtual machine
	while (halt)
	{

		// FETCH
		IR.op = pas[PC];
		IR.l = pas[PC + 1];
		IR.m = pas[PC + 2];
		PC += 3;

		// EXECUTE
		switch (IR.op)
		{
		case 1: // INC
			SP -= IR.m;
			break;

		case 2: // OPR
			switch (IR.m)
			{
			case 0: // RTN
				SP = BP + 1;
				BP = pas[SP - 2];
				PC = pas[SP - 3];
				break;
			case 1: // ADD
				pas[SP + 1] += pas[SP];
				SP++;
				break;
			case 2: // SUB
				pas[SP + 1] -= pas[SP];
				SP++;
				break;
			case 3: // MUL
				pas[SP + 1] *= pas[SP];
				SP++;
				break;
			case 4: // DIV
				pas[SP + 1] /= pas[SP];
				SP++;
				break;
			case 5: // EQL
				pas[SP + 1] = (pas[SP + 1] == pas[SP]);
				SP++;
				break;
			case 6: // NEQ
				pas[SP + 1] = (pas[SP + 1] != pas[SP]);
				SP++;
				break;
			case 7: // LSS
				pas[SP + 1] = (pas[SP + 1] < pas[SP]);
				SP++;
				break;
			case 8: // LEQ
				pas[SP + 1] = (pas[SP + 1] <= pas[SP]);
				SP++;
				break;
			case 9: // GTR
				pas[SP + 1] = (pas[SP + 1] > pas[SP]);
				SP++;
				break;
			case 10: // GEQ
				pas[SP + 1] = (pas[SP + 1] >= pas[SP]);
				SP++;
				break;
			}
			break;

		case 3: // LOD
			SP--;
			pas[SP] = pas[base(IR.l, BP) - IR.m];
			break;

		case 4: // STO
			pas[base(IR.l, BP) - IR.m] = pas[SP];
			SP++;
			break;

		case 5: // CAL
			pas[SP - 1] = base(IR.l, BP);
			pas[SP - 2] = BP;
			pas[SP - 3] = PC;
			BP = SP - 1;
			PC = IR.m;
			break;

		case 6: // LIT
			SP--;
			pas[SP] = IR.m;
			break;

		case 7: // JMP
			PC = IR.m;
			break;

		case 8: // JPC
			if (pas[SP] == 0)
				PC = IR.m;
			SP++;
			break;
		case 9: // SYS
			switch (IR.m)
			{
			case 1:
				// Output the top of the stack
				printf("Output result is: %d\n", pas[SP]);
				SP++;
				break;
			case 2:
				// Input an integer and push it onto the stack
				printf("Please Enter an Integer: ");
				SP--;
				scanf("%d", &pas[SP]);
				break;
			case 3:
				// Halt the program
				halt = 0;
				break;
			}
			break;
		}
		// Print the current state of the virtual machine after execution of instructions
		printState();
	}
	return 0;
}
