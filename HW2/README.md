COP 3402: Systems Software
Homework 2: Lexical Analyzer 
by George Morillo & Arturo Lara

Description: 
Lexical analyzer designed for PL/0, capable of identifying errors, reading from an input file, and producing an output file

Instructions on how to run:
1. Go to directory on console terminal where file is located 
2. Use command gcc -Wall lex.c -o lex.exe
3. After it's compiled use command ./lex.exe [input_filename].txt
4. Read the output file names output.txt

Instructions on input formatting:
1. You can use individual letters and digits
2. You can combine digits and letters to form names and numbers
3. Special symbols include /, +, =, -, *, ;, :, >, <, ), (, 
4. Reserved words: "const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"
5. Reserved tokens: constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, fisym, thensym, elsesym, whilesym, dosym, readsym, writesym
6. Errors to catch: Name too long, Number too long, and Invalid symbol
