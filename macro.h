#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100
#define NAME_SIZE 20
#define BUFF_SIZE 256

typedef struct {
    char name[NAME_SIZE];
    char *params[TABLE_SIZE];
    int param_count;
    int start_index;
    int end_index;
} Macro;
typedef Macro* pMacro;

Macro* create_new_macro(int pos, char* buffer, unsigned start, char* str);
void replace_string(char *string, char *from, char *to);
void replace_macro(char strings[][BUFF_SIZE], int stringsCount, Macro* macro, char *call_line, FILE *output_file);
void macro_process(FILE* in);
