// George Morillo & Arturo Lara
// COP 3402: Systems Software
// Homework 2: Lexical Analyzer 
// George Morillo's contributions: Creating functions to read input, handle characters, numbers, and setting token values and reserved words
// Arturo Lara's contributions: Implementing error handling, special symbol handling, structure for tokens/errors, and function to generate output

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

// Struct for assigning numerical values to each token
typedef enum {
    skipsym = 1, identsym, numbersym, plussym, minussym, multsym,
    slashsym, fisym, eqlsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym, errorsym = -1
} TokenType;

// Struct to identify tokens and errors
typedef struct {
    TokenType type;
    char lexeme[MAX_IDENT_LEN + 1];
    char error_msg[MAX_ERROR_MSG];
} Token;

// Global variables
Token tokens[MAX_LEXEMES];
int token_count = 0;

// Array of reserved words
char* reserved_words[] = {
    "const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"
};

// Array of reserved tokens
TokenType reserved_tokens[] = {
    constsym, varsym, procsym, callsym, beginsym, endsym,
    ifsym, fisym, thensym, elsesym, whilesym, dosym, readsym, writesym
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
    for (int i = 0; i < sizeof(reserved_words) / sizeof(char*); i++) {
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
        return;
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
                    if (c == '*' && (c = fgetc(file)) == '/')
                        break;
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
                    if (next == '=') {
                        strcpy(sym, "<=");
                        add_token(leqsym, sym);
                    } else if (next == '>') {
                        strcpy(sym, "<>");
                        add_token(neqsym, sym);
                    } else {
                        if (next != EOF) ungetc(next, file);
                        add_token(lessym, sym);
                    }
                    break;
                case '>':
                    next = fgetc(file);
                    if (next == '=') {
                        strcpy(sym, ">=");
                        add_token(geqsym, sym);
                    } else {
                        if (next != EOF) ungetc(next, file);
                        add_token(gtrsym, sym);
                    }
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

// Function to print results to output file
void write_output(const char* input_filename, const char* output_filename) {
    FILE* in = fopen(input_filename, "r");
    FILE* out = fopen(output_filename, "w");
    if (!in || !out) {
        printf("Error opening files.\n");
        return;
    }
    
    // Print information from 
    fprintf(out, "Source Program:\n\n");
    char ch;
    while ((ch = fgetc(in)) != EOF)
        fputc(ch, out);
    fprintf(out, "\n");

    // Print lexeme table
    fprintf(out, "\nLexeme Table:\n\n");
    fprintf(out, "lexeme\t\ttoken type\n");
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == errorsym)
            fprintf(out, "%-10s	%s\n", tokens[i].lexeme, tokens[i].error_msg);
        else
            fprintf(out, "%-10s	%d\n", tokens[i].lexeme, tokens[i].type);
    }

    // Check if there are any errors
    int has_error = 0;
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == errorsym) {
            has_error = 1;
            break;
        }
    }

    // If no errors, print token list
    if (!has_error) {
        fprintf(out, "\nToken List:\n");
        for (int i = 0; i < token_count; i++) {
            fprintf(out, "%d ", tokens[i].type);
            if (tokens[i].type == identsym || tokens[i].type == numbersym)
                fprintf(out, "%s ", tokens[i].lexeme);
        }
        fprintf(out, "\n");
    }

    fclose(in);
    fclose(out);
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: ./main.exe input.txt\n");
        return 1;
    }

    const char* output_filename = "output.txt";
    scan(argv[1]);
    write_output(argv[1], output_filename);
    return 0;
}
