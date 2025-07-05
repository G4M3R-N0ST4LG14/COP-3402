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
3. Special symbols include /, +, =, -, *, ;, :, >, <, ), (, 
4. Reserved words: "const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"
5. Reserved tokens: constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, fisym, thensym, elsesym, whilesym, dosym, readsym, writesym
6. Errors to catch: Name too long, Number too long, and Invalid symbol