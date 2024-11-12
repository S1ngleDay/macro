#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#define MAX_LINE_LENGTH 256

typedef struct {
    char* label;
    char* mnemonic;
    char* operand;
    char* comment;
} line_e;

typedef struct {
    line_e** entries;
    int size;
} line_t;

line_t * create_line_t();
void freeLineTable(line_t * table);
void add_entry(line_t *table, char *label, char *mnemonic, char *operand, char *comment);
void get_line_t(FILE* in, line_t *line_table);

#endif // LEXER_H