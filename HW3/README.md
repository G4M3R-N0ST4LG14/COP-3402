COP 3402: Systems Software
Homework 3: Tiny PL/0 Compiler
by George Morillo & Arturo Lara

Description: The objective of this project is to build off of the previous project form HW2, lex.c. We are to copy the code used from that to create an expanded program that takes the output from lex.c and uses a parser and code generator to create an assembly code file that is readable by HW1's vm.c program.

Instructions on how to run:
1. Go to directory on console terminal where file is located 
2. Use command gcc -Wall parsercodegen.c -o parsercodegen.exe
3. After it's compiled use command ./parsercodegen.exe [input_filename].txt
4. If the inputed file is grammatically correct the program will write to the terminal the assembly code and symbol table that is generated
5. The output.txt file will be generated and when used with vm.exe should produce a working output

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