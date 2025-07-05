// George Morillo & Arturo Lara
// COP 3402: Systems Software
// Homework 3: Tiny PL/0 Compiler
// George Morillo's contributions: Developing the main parser functions, variable declarations and formatting the terminal output
// Arturo Lara's contributions: Developing the code generator process, constant delcarations and benchmarking input and output files along vm.c

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constants
#define MAX_IDENT_LEN 11
#define MAX_NUM_LEN 5
#define MAX_LEXEMES 1000
#define MAX_ERROR_MSG 50
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_CODE_LENGTH 500

// Majority of everything in this first section is the same as previous project lex.c

// Struct for assigning numerical values to each token
typedef enum {
    oddsym = 1, identsym, numbersym, plussym, minussym, multsym,
    slashsym, eqlsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, constsym, varsym, writesym, readsym,
    whensym, fi, errorsym = -1
} TokenType;

// Struct to identify tokens and errors
typedef struct {
    TokenType type;
    char lexeme[MAX_IDENT_LEN + 1];
    char error_msg[MAX_ERROR_MSG];
} Token;

// Global Variables
Token tokens[MAX_LEXEMES];
int token_count = 0;
int token_index = 0;
Token current_token;

// Struct for symbols
typedef struct {
    int kind;      // 1 = const, 2 = var
    char name[12];
    int val;
    int level;
    int addr;
    int mark;
} symbol;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbol_count = 0;

// Struct for instructions
typedef struct {
    int op;
    int l;
    int m;
} instruction;
instruction code[MAX_CODE_LENGTH];
int code_index = 0;

// Array of reserved words
char* reserved_words[] = {
    "const", "var", "begin", "end", "if", "then", "while", "do",
    "read", "write", "when", "fi", "odd"
};

// Array of operators names
const char* op_names[] = {
    "", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SYS"
};

// Array of reserved tokens
TokenType reserved_tokens[] = {
    constsym, varsym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, readsym, writesym, whensym, fi, oddsym
};

// Function to add a token
void add_token(TokenType type, const char* lexeme) {
    if (token_count >= MAX_LEXEMES) return;
    tokens[token_count].type = type;
    strncpy(tokens[token_count].lexeme, lexeme, MAX_IDENT_LEN);
    tokens[token_count].error_msg[0] = '\0';
    token_count++;
}

// Function to flag for errors
void add_error_token(const char* lexeme, const char* msg) {
    if (token_count >= MAX_LEXEMES) return;
    tokens[token_count].type = errorsym;
    strncpy(tokens[token_count].lexeme, lexeme, MAX_IDENT_LEN);
    strncpy(tokens[token_count].error_msg, msg, MAX_ERROR_MSG - 1);
    token_count++;
}

// Function to flag reserved words
int is_reserved(const char* word) {
    for (int i = 0; i < sizeof(reserved_words)/sizeof(char*); i++) {
        if (strcmp(reserved_words[i], word) == 0)
            return reserved_tokens[i];
    }
    return 0;
}

// Function to scan input and flag its identity
void scan(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file.\n");
        exit(1);
    }

    char c;
    while ((c = fgetc(file)) != EOF) {

        // Ignore white space
        if (isspace(c)) 
            continue;

        // Character handling  
        else if (isalpha(c)) {
            char buffer[MAX_IDENT_LEN + 2] = {0};
            int i = 0;
            buffer[i++] = c;
            while ((c = fgetc(file)) != EOF && isalnum(c)) {
                if (i < MAX_IDENT_LEN + 1)
                    buffer[i++] = c;
            }
            buffer[i] = '\0';
            if (c != EOF) 
                ungetc(c, file);

            // Check for name length error
            if (strlen(buffer) > MAX_IDENT_LEN) {
                add_error_token(buffer, "Error: Name too long");
                continue;
            }
            int res = is_reserved(buffer);
            if (res)
                add_token(res, buffer);
            else
                add_token(identsym, buffer);

        // Numbers handling  
        } else if (isdigit(c)) {
            char buffer[MAX_NUM_LEN + 2] = {0};
            int i = 0;
            buffer[i++] = c;
            while ((c = fgetc(file)) != EOF && isdigit(c)) {
                if (i < MAX_NUM_LEN + 1)
                    buffer[i++] = c;
            }
            buffer[i] = '\0';
            if (c != EOF) ungetc(c, file);
            // Check for number length error
            if (strlen(buffer) > MAX_NUM_LEN) {
                add_error_token(buffer, "Error: Number too long");
                continue;
            }
            add_token(numbersym, buffer);

        // Handle '/' and comments
        } else if (c == '/') {
            char next = fgetc(file);
            if (next == '*') {
                while ((c = fgetc(file)) != EOF) {
                    if (c == '*' && (c = fgetc(file)) == '/') break;
                }
            } else {
                if (next != EOF) ungetc(next, file);
                char sym[3] = {c, '\0', '\0'};
                add_token(slashsym, sym);
            }
            
        // Special symbol handling  
        } else {
            char next;
            char sym[3] = {c, '\0', '\0'};
            switch (c) {
                case '+': add_token(plussym, sym); break;
                case '-': add_token(minussym, sym); break;
                case '*': add_token(multsym, sym); break;
                case '=': add_token(eqlsym, sym); break;
                case '<':
                    next = fgetc(file);
                    if (next == '=') { strcpy(sym, "<="); add_token(leqsym, sym); }
                    else if (next == '>') { strcpy(sym, "<>"); add_token(neqsym, sym); }
                    else { if (next != EOF) ungetc(next, file); add_token(lessym, sym); }
                    break;
                case '>':
                    next = fgetc(file);
                    if (next == '=') { strcpy(sym, ">="); add_token(geqsym, sym); }
                    else { if (next != EOF) ungetc(next, file); add_token(gtrsym, sym); }
                    break;
                case '(':
                    add_token(lparentsym, sym); break;
                case ')':
                    add_token(rparentsym, sym); break;
                case ',':
                    add_token(commasym, sym); break;
                case ';':
                    add_token(semicolonsym, sym); break;
                case '.':
                    add_token(periodsym, sym); break;
                case ':':
                    next = fgetc(file);
                    if (next == '=') {
                        strcpy(sym, ":=");
                        add_token(becomessym, sym);
                    } else {
                        if (next != EOF) ungetc(next, file);
                        add_error_token(sym, "Error: Invalid symbol");
                    }
                    break;
                default:
                    add_error_token(sym, "Error: Invalid symbol");
                    break;
            }
        }
    }

    fclose(file);
}

// Start of of parser and code generator

// Function to get the next token from the token stream
void getNextToken() {
    if (token_index < token_count)
        current_token = tokens[token_index++];
}

// Functions to print error message and stop the program
void error(const char* msg) {
    printf("Error: %s\n", msg);
    exit(1);
}

// Function to emit an instruction
void emit(int op, int l, int m) {
    code[code_index].op = op;
    code[code_index].l = l;
    code[code_index].m = m;
    code_index++;
}

// Function to search the symbol table for a name 
int findSymbol(const char* name) {
    for (int i = symbol_count - 1; i >= 0; i--) {
        if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].mark == 1) // Found
            return i;
    }
    return -1; // Not found
}

// Function that handle variable declarations 
int varDeclaration() {
    int numVars = 0;
    if (current_token.type == varsym) {
        do {
            getNextToken();

            if (current_token.type != identsym)
                error("Var must be followed by an identifier");

            if (findSymbol(current_token.lexeme) != -1)
                error("Symbol name has already been declared");

            // Add var to symbol table
            symbol_table[symbol_count].kind = 2;
            strcpy(symbol_table[symbol_count].name, current_token.lexeme);
            symbol_table[symbol_count].level = 0;
            symbol_table[symbol_count].addr = symbol_count + 3;
            symbol_table[symbol_count].mark = 1;
            symbol_count++;
            numVars++;

            getNextToken();
        } while (current_token.type == commasym);

        if (current_token.type != semicolonsym)
            error("Var declaration must be followed by a semicolon");

        getNextToken();
    }
    return numVars;
}

// Function to handle constant declarations
void constDeclaration() {
    if (current_token.type == constsym) {
        do {
            getNextToken();

            if (current_token.type != identsym)
                error("Const must be followed by identifier");

            if (findSymbol(current_token.lexeme) != -1)
                error("Symbol name has already been declared");

            // Save identifier name
            char name[MAX_IDENT_LEN + 1];
            strncpy(name, current_token.lexeme, MAX_IDENT_LEN);

            getNextToken();
            if (current_token.type != eqlsym)
                error("Constants must be assigned with =");

            getNextToken();
            if (current_token.type != numbersym)
                error("Constants must be assigned an integer value");

            // Add const to symbol table
            symbol_table[symbol_count].kind = 1;
            strncpy(symbol_table[symbol_count].name, name, MAX_IDENT_LEN);
            symbol_table[symbol_count].val = atoi(current_token.lexeme);
            symbol_table[symbol_count].level = 0;
            symbol_table[symbol_count].addr = 0; // not used
            symbol_table[symbol_count].mark = 1;
            symbol_count++;

            getNextToken();
        } while (current_token.type == commasym);

        if (current_token.type != semicolonsym)
            error("Constant and variable declarations must be followed by a semicolon");

        getNextToken();
    }
}

// Function expression prototype
void expression();

// Function to parse a factor
void factor() {
    if (current_token.type == identsym) {
        int idx = findSymbol(current_token.lexeme);
        if (idx == -1)
            error("Undeclared identifier");
        if (symbol_table[idx].kind == 1)
            emit(1, 0, symbol_table[idx].val);
        else
            emit(3, 0, symbol_table[idx].addr);
        getNextToken();
    } else if (current_token.type == numbersym) {
        emit(1, 0, atoi(current_token.lexeme));
        getNextToken();
    } else if (current_token.type == lparentsym) {
        getNextToken();
        expression();
        if (current_token.type != rparentsym)
            error("Right parenthesis must follow left parenthesis");
        getNextToken();
    } else {
        error("Arithmetic equations must contain operands, parentheses, numbers, or symbols");
    }
}

// Fucntion to parse a term
void term() {
    factor();
    while (current_token.type == multsym || current_token.type == slashsym) {
        TokenType op = current_token.type;
        getNextToken();
        factor();
        emit(2, 0, (op == multsym) ? 3 : 4);
    }
}

// Function to parse an expression
void expression() {
    term();
    while (current_token.type == plussym || current_token.type == minussym) {
        TokenType op = current_token.type;
        getNextToken();
        term();
        emit(2, 0, (op == plussym) ? 1 : 2);
    }
}

// Function to parse an statement
void statement() {
    if (current_token.type == identsym) {
        int idx = findSymbol(current_token.lexeme);
        if (idx == -1) error("Undeclared identifier");
        if (symbol_table[idx].kind != 2) error("Only variable values may be altered");
        getNextToken();
        if (current_token.type != becomessym)
            error("Assignment statements must use :=");
        getNextToken();
        expression();
        emit(4, 0, symbol_table[idx].addr);
    } else if (current_token.type == beginsym) {
        do {
            getNextToken();
            statement();
        } while (current_token.type == semicolonsym);
        if (current_token.type != endsym)
            error("Begin must be followed by end");
        getNextToken();
    } else if (current_token.type == readsym) {
        getNextToken();
        if (current_token.type != identsym)
            error("Read must be followed by identifier");
        int idx = findSymbol(current_token.lexeme);
        if (idx == -1 || symbol_table[idx].kind != 2)
            error("Undeclared or invalid identifier in read");
        getNextToken();
        emit(9, 0, 2);
        emit(4, 0, symbol_table[idx].addr);
    } else if (current_token.type == writesym) {
        getNextToken();
        expression();
        emit(9, 0, 1);
    }
}

// Function to parse a block
void block() {
    constDeclaration();
    int numVars = varDeclaration();
    emit(6, 0, 3 + numVars); // INC
    statement();
}

// Function of entry point for parsing and code generation
void program() {
    emit(7, 0, 0); // JMP placeholder
    block();
    if (current_token.type != periodsym)
        error("Program must end with period");
    emit(9, 0, 3); // SYS HALT
    code[0].m = 1;
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    scan(argv[1]);

    // Error handeling 
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == errorsym) {
            printf("Error: %s\n", tokens[i].error_msg);
            return 1;
        }
    }

    // Begin parsing
    getNextToken();
    emit(7, 0, 13); // JMP 0 13
    block();

    // Error for missing period (.)
    if (current_token.type != periodsym)
        error("Program must end with a period.");

    emit(9, 0, 3); // SYS HALT

    // Create output file
    // In order to process this file use vm.c
    FILE *outfile = fopen("assemblyOutput.txt", "w");
    if (!outfile) {
        printf("Error: Could not open output file.\n");
        exit(1);
    }

    // Printing format
    printf("\nAssembly Code:\n");
    printf("Line\tOP\tL\tM\n");
    for (int i = 0; i < code_index; i++) {
        // Prints the assembly code formated as such:
        // Line OP L M
        // Where OP is the operation code, L is the level, and M is the modifier
        printf("%-5d\t%-4s\t%-2d\t%-3d\n", i, op_names[code[i].op], code[i].l, code[i].m);
        // Prints to the output file the assembly code in digit format
        fprintf(outfile, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
    }
    fclose(outfile);

    // Printing Symbol table
    printf("\nSymbol Table:\n");
    printf("Kind | Name       | Value | Level | Address | Mark\n");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < symbol_count; i++) {
        symbol s = symbol_table[i];
        printf("%-4d | %-10s | %-5d | %-5d | %-7d | %-4d\n", s.kind, s.name, s.val, s.level, s.addr, s.mark);
    }

    return 0;
}

