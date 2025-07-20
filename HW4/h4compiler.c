// George Morillo & Arturo Lara
// COP 3402: Systems Software
// Homework 4: Tiny PL/0 Compiler with Procedures, Calls, If-Else-Fi, Mod, and Lexical Levels
// George Morillo's contributions: Implementing new grammar elements procedure and call
// Arturo Lara's contributions: Implementing new grammar elements else clause in if statements and modulo operations in vm.c

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

// Majority of everything in this first section is the same as previous project parssercodegen.c

// Struct for assigning numerical values to each token
typedef enum {
  modsym = 1, identsym, numbersym, plussym, minussym, multsym,
  slashsym, fisym, eqlsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whensym, dosym, callsym, constsym, varsym, procsym,
  writesym, readsym, elsesym, errorsym = -1
} TokenType;

// Struct to identify tokens and errors
typedef struct {
  TokenType type;
  char lexeme[MAX_IDENT_LEN + 1];
  char error_msg[MAX_ERROR_MSG];
} Token;

// Struct for symbols
typedef struct {
  int kind;
  char name[10];
  int val;
  int level;
  int addr;
  int mark;
} symbol;

// Struct for instructions
typedef struct {
  int op;
  int l;
  int m;
} instruction;

// Global Variables
Token tokens[MAX_LEXEMES];
int token_count = 0, token_index = 0;
Token current_token;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbol_count = 0;
instruction code[MAX_CODE_LENGTH];
int code_index = 0;
int current_level = 0;

// Added 'when' and 'mod'
char* reserved_words[] = {
  "const", "var", "begin", "end", "if", "then", "when", "do",
  "read", "write", "call", "procedure", "fi", "else", "mod"
};

// Aligned reserved token values with updated grammar
TokenType reserved_tokens[] = {
  constsym, varsym, beginsym, endsym, ifsym, thensym, whensym, dosym,
  readsym, writesym, callsym, procsym, fisym, elsesym, modsym
};

// Array of operator names
const char* op_names[] = {
  "", "INC", "OPR", "LOD", "STO", "CAL", "LIT", "JMP", "JPC", "SYS"
};

//  Token & Scanner Functions
void add_token(TokenType type, const char* lexeme);
void add_error_token(const char* lexeme, const char* msg);
int is_reserved(const char* word);
void scan(const char* filename);

// Parser Functions
void getNextToken();
void error(const char* msg);
void emit(int op, int l, int m);
int findSymbol(const char* name);
void expression();
void factor();
void term();
void condition();
void statement();
void constDeclaration();
int varDeclaration();
void procedureDeclaration();
void block();
void program();


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

//Function to flag reserved words
int is_reserved(const char* word) {
  for (int i = 0; i < sizeof(reserved_words)/sizeof(char*); i++)
    if (strcmp(reserved_words[i], word) == 0)
      return reserved_tokens[i];
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
    
    // Ignore whitespace
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
      if (c != EOF) ungetc(c, file);

      int res = is_reserved(buffer);
      if (res)
        add_token(res, buffer);
      else
        add_token(identsym, buffer);
    }

    // Numbers handling
    else if (isdigit(c)) {
      char buffer[MAX_NUM_LEN + 2] = {0};
      int i = 0;
      buffer[i++] = c;

      while ((c = fgetc(file)) != EOF && isdigit(c)) {
        if (i < MAX_NUM_LEN + 1)
          buffer[i++] = c;
      }
      buffer[i] = '\0';
      if (c != EOF) ungetc(c, file);

      add_token(numbersym, buffer);
    }

    // Handle '/' and block comments
    else if (c == '/') {
      char next = fgetc(file);
      if (next == '*') {
        while ((c = fgetc(file)) != EOF) {
          if (c == '*' && fgetc(file) == '/') break;
        }
      } else {
        if (next != EOF) ungetc(next, file);
        add_token(slashsym, "/");
      }
    }

    // Special symbols
    else {
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
            add_error_token(sym, "Invalid symbol");
          }
          break;
        default:
          add_error_token(sym, "Invalid symbol");
          break;
      }
    }
  }

  fclose(file);
}


// Parser Functions

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

// Fcuntion to emit an instruction
void emit(int op, int l, int m) {
  if (code_index >= MAX_CODE_LENGTH)
    error("Code size exceeded");
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

void expression();
void factor();
void term();
void condition();
void statement();
void constDeclaration();
int varDeclaration();
void procedureDeclaration();
void block();
void program();

// Function to handle constant declarations
void constDeclaration() {
  if (current_token.type == constsym) {
    do {
      getNextToken();
      if (current_token.type != identsym) 
        error("Const must be followed by identifier");

      if (findSymbol(current_token.lexeme) != -1)
        error("Symbol name has already been declared");

      char name[MAX_IDENT_LEN + 1];
      strcpy(name, current_token.lexeme);

      getNextToken();
      if (current_token.type != eqlsym) 
        error("Constants must be assigned with =");
      getNextToken();
      if (current_token.type != numbersym) 
        error("Constants must be assigned an integer value");

      symbol_table[symbol_count++] = (symbol){1, "", atoi(current_token.lexeme), current_level, 0, 1};
      strcpy(symbol_table[symbol_count - 1].name, name);

      getNextToken();
    } while (current_token.type == commasym);

    if (current_token.type != semicolonsym)
      error("Constant and variable declarations must be followed by a semicolon");

    getNextToken();
  }
}

// Function to handle var declarations
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
      symbol_table[symbol_count].val = 0;
      symbol_table[symbol_count].level = current_level;
      symbol_table[symbol_count].addr = numVars + 3;  
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

// Function to handle procedure declarations
void procedureDeclaration() {
  while (current_token.type == procsym) {
    getNextToken();
    if (current_token.type != identsym) error("Procedure must be followed by identifier");

    char name[MAX_IDENT_LEN + 1];
    strcpy(name, current_token.lexeme);

    symbol_table[symbol_count++] = (symbol){3, "", 0, current_level, code_index, 1};
    strcpy(symbol_table[symbol_count - 1].name, name);

    getNextToken();
    if (current_token.type != semicolonsym) error("Semicolon expected after procedure name");
    getNextToken();

    current_level++;
    block();
    current_level--;

    if (current_token.type != semicolonsym) error("Semicolon expected after procedure body");
    getNextToken();
  }
}

// Function to parse a factor
void factor() {
  if (current_token.type == identsym) {
    int idx = findSymbol(current_token.lexeme);
    if (idx == -1) 
      error("Undeclared identifier");
    if (symbol_table[idx].kind == 1)
      emit(6, 0, symbol_table[idx].val); 
    else
      emit(3, current_level - symbol_table[idx].level, symbol_table[idx].addr); 
    getNextToken();
  } else if (current_token.type == numbersym) {
    emit(6, 0, atoi(current_token.lexeme)); 
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

// Function to parse a term
void term() {
  factor();
  while (current_token.type == multsym || current_token.type == slashsym || current_token.type == modsym) {
    TokenType op = current_token.type;
    getNextToken();
    factor();
    if (op == multsym) emit(2, 0, 3); 
    else if (op == slashsym) emit(2, 0, 4); 
    else emit(2, 0, 11); 
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

// Function to parse a condition
void condition() {
  expression();
  int op;
  switch (current_token.type) {
    case eqlsym: op = 5; break;
    case neqsym: op = 6; break;
    case lessym: op = 7; break;
    case leqsym: op = 8; break;
    case gtrsym: op = 9; break;
    case geqsym: op = 10; break;
    default: error("Relational operator expected");
  }
  getNextToken();
  expression();
  emit(2, 0, op);
}

// Function to parse an statement
void statement() {
    if (current_token.type == identsym) {
      int idx = findSymbol(current_token.lexeme);
      if (idx == -1)
        error("Undeclared identifier");

      if (symbol_table[idx].kind != 2)
        error("Only variable values may be altered");

      getNextToken();
      if (current_token.type != becomessym)
        error("Assignment statements must use :=");

      getNextToken();
      expression();
      emit(4, current_level - symbol_table[idx].level, symbol_table[idx].addr);

  }
  else if (current_token.type == callsym) {
    getNextToken();
    if (current_token.type != identsym) error("Call must be followed by identifier");
    int idx = findSymbol(current_token.lexeme);
    if (idx == -1) error("Undeclared procedure");
    if (symbol_table[idx].kind != 3) error("Call to non-procedure");

    emit(5, current_level - symbol_table[idx].level, symbol_table[idx].addr); // CAL
    getNextToken();
  }
  else if (current_token.type == beginsym) {
    do {
      getNextToken();
      statement();
    } while (current_token.type == semicolonsym);
    if (current_token.type != endsym) error("end expected");
    getNextToken();
  }
  else if (current_token.type == ifsym) {
    getNextToken();
    condition();
    if (current_token.type != thensym) error("then expected");
    getNextToken();
    int jpcIdx = code_index;
    emit(8, 0, 0); // JPC placeholder

    statement();

    if (current_token.type != elsesym) error("else expected");
    int jmpIdx = code_index;
    emit(7, 0, 0); // JMP placeholder

    code[jpcIdx].m = code_index;

    getNextToken();
    statement();

    if (current_token.type != fisym) error("fi expected");
    code[jmpIdx].m = code_index;
    getNextToken();
  }
  else if (current_token.type == whensym) {
    getNextToken();
    int loopIdx = code_index;
    condition();
    if (current_token.type != dosym) error("do expected");
    getNextToken();
    int jpcIdx = code_index;
    emit(8, 0, 0);
    statement();
    emit(7, 0, loopIdx); 
    code[jpcIdx].m = code_index;
  }
  else if (current_token.type == readsym) {
    getNextToken();
    if (current_token.type != identsym) error("Identifier expected");
      int idx = findSymbol(current_token.lexeme);
    if (idx == -1 || symbol_table[idx].kind != 2)
      error("Invalid identifier in read");
    getNextToken();
    emit(9, 0, 2); 
    emit(4, current_level - symbol_table[idx].level, symbol_table[idx].addr); 
  }
  else if (current_token.type == writesym) {
    getNextToken();
    expression();
    emit(9, 0, 1); 
  }
  else {
    if (current_token.type == becomessym) {
        error("Identifier expected before ':='");
    } else {
        error("Invalid statement.");
    }
  }
}

// Function to parse a block
void block() {
  constDeclaration();
  int numVars = varDeclaration();
  procedureDeclaration();

  emit(1, 0, 3 + numVars); 

  statement();
}

// Function of entry point for parsing and code generation
// program(): initializes with JMP, patches JMP, emits HALT; HW4-specific
void program() {
  emit(7, 0, 0); // JMP 0 ? placeholder
  int jmpIndex = 0;
  block();
  if (current_token.type != periodsym)
    error("Program must end with a period.");
  emit(9, 0, 3); // SYS 0 3 halt
  code[jmpIndex].m = code_index; // Patch JMP to actual start of code
}


// Main function
int main(int argc, char* argv[]) {
  if (argc < 2) return 1;
  scan(argv[1]);
  for (int i = 0; i < token_count; i++)
    if (tokens[i].type == errorsym) {
      printf("Error: %s\n", tokens[i].error_msg);
      return 1;
    }
  getNextToken();
  program();
  printf("No errors, program is syntactically correct.\n\n");
  printf("Assembly Code:\nLine\tOP\tL\tM\n");
  FILE* out = fopen("elf.txt", "w");
  for (int i = 0; i < code_index; i++) {
    printf("%d\t%s\t%d\t%d\n", i, op_names[code[i].op], code[i].l, code[i].m);
    fprintf(out, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
  }
  fclose(out);
  printf("\nSymbol Table:\nKind | Name       | Value | Level | Address | Mark\n");
  printf("---------------------------------------------------\n");
  for (int i = 0; i < symbol_count; i++) {
    symbol s = symbol_table[i];
    printf("%-4d | %-10s | %-5d | %-5d | %-7d | %-4d\n", s.kind, s.name, s.val, s.level, s.addr, s.mark);
  }
  return 0;
}