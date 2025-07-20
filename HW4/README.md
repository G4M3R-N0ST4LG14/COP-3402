COP 3402: Systems Software
Homework 4: PL/0 Compiler and VM
by George Morillo & Arturo Lara

Description: Implement new grammar elements involving procedure, call, and else clause in if statements in Homework #3's parsercodegen.c program and modulo operations in Homework #1's vm.c

Instructions on how to run:
1. Go to directory on console terminal where file is located 
2. Use command gcc -Wall h4compiler.c -o h4compiler.exe
3. After it's compiled use command ./compiler.exe [file_directory]/[input_filename].txt
4. If the inputed file is grammatically correct the program will write to the terminal the assembly code and symbol table that is generated
5. The elf.txt file will be generated and when used with vm.exe should produce a working output (use the vm.c that was uploaded with h4compiler.c)

Instructions on input formatting:
1. You can use individual letters and digits
2. You can combine digits and letters to form names and numbers
3. Special symbols include /, +, =, -, *, ;, :, >, <, ), (
4. Reserved words: "const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"
5. Reserved tokens: constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, fisym, thensym, elsesym, whilesym, dosym, readsym, writesym

Error Handling:
In the event that there was a mistake following the instructions above the program will prevent an output from being created and instead write out an error message that details the cause of the error. Note that the location of the error will not be detailed. 
Here is a list of errors that can occur:
* Program must end with period
* Const, var, and read keywords must be followed by identifier
* Symbol name has already been declared
* Constants must be assigned with =
* Constants must be assigned an integer value
* Constant and variable declarations must be followed by a semicolon
* Undeclared identifier
* Only variable values may be altered
* Assignment statements must use :=
* Begin must be followed by end
* If must be followed by then
* While must be followed by do
* Condition must contain comparison operator
* Right parenthesis must follow left parenthesis
* Arithmetic equations must contain operands, parentheses, numbers, or symbols