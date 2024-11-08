#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include "lexer.h"
#define MAX_LINE_LENGTH 256

int main() {
    line_t *a = create_line_t();
    get_line_t(a);
    for (int i = 0; i < a->size; i++) {
        printf("lable: %s | mnemo: %s | oper: %s | comm: %s |\n", a->entries[i]->label, a->entries[i]->mnemonic, a->entries[i]->operand, a->entries[i]->comment);
    }
    return 0;
}
