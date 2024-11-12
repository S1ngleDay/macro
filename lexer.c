#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"

#define MAX_LINE_LENGTH 256

line_t *create_line_t() {
    line_t *table = (line_t *)malloc(sizeof(line_t));
    if (table == NULL) {
        perror("Failed to allocate memory for line_t");
        exit(EXIT_FAILURE);
    }
    table->size = 0;
    table->entries = NULL;
    return table;
}

void freeLineTable(line_t *table) {
    if (table == NULL) return;
    for (int i = 0; i < table->size; i++) {
        free(table->entries[i]->label);
        free(table->entries[i]->mnemonic);
        free(table->entries[i]->operand);
        free(table->entries[i]->comment);
        free(table->entries[i]);
    }
    free(table->entries);
    free(table);
}

void add_entry(line_t *table, const char *label, const char *mnemonic, const char *operand, const char *comment) {
    line_e *entry = (line_e *)malloc(sizeof(line_e));
    if (entry == NULL) {
        perror("Failed to allocate memory for line_e");
        exit(EXIT_FAILURE);
    }
    entry->label = strdup(label ? label : "");
    entry->mnemonic = strdup(mnemonic ? mnemonic : "");
    entry->operand = strdup(operand ? operand : "");
    entry->comment = strdup(comment ? comment : "");

    table->entries = (line_e **)realloc(table->entries, sizeof(line_e *) * (table->size + 1));
    if (table->entries == NULL) {
        perror("Failed to allocate memory for entries");
        exit(EXIT_FAILURE);
    }
    table->entries[table->size] = entry;
    table->size++;
}

void get_line_t(FILE* in, line_t *line_table) {
    char input_string[MAX_LINE_LENGTH];
    char *label = NULL, *mnemonic = NULL, *operand = NULL, *comment = NULL;

    if (in == NULL) {
        perror("Failed to open file");
        return;
    }

    while (fgets(input_string, MAX_LINE_LENGTH, in) != NULL) {
        input_string[strcspn(input_string, "\n")] = '\0';

        // Пропускаем пустые строки и строки, состоящие только из пробелов
        if (input_string[0] == '\0' || strspn(input_string, " \t") == strlen(input_string)) continue;

        // Если строка начинается с '*', то вся строка - это комментарий
        if (input_string[0] == '*') {
            label = mnemonic = operand = "";  // Метка, мнемоника и операнд пустые
            comment = input_string + 1;  // Сохраняем комментарий, пропуская символ '*'
        } else {
            char *token = input_string;

            // Ищем первую часть как метку (если начинается с буквы)
            if (isalpha(*token)) {
                label = token;
                while (*token && !isspace(*token)) token++;
                if (*token) *token++ = '\0';  // Завершаем метку нулем и переходим к следующему токену
            } else {
                label = "";  // Нет метки
            }

            // Пропускаем пробелы и табуляции
            while (*token && isspace(*token)) token++;

            // Следующий токен — мнемоника
            mnemonic = token;
            while (*token && !isspace(*token)) token++;
            if (*token) *token++ = '\0';

            // Пропускаем пробелы и табуляции
            while (*token && isspace(*token)) token++;

            // Следующий токен — операнд
            operand = token;
            while (*token && *token != '*' && *token != ';' && !isspace(*token)) token++;
            if (*token) {
                if (*token == '*' || *token == ';') {
                    *token++ = '\0';  // Завершаем операнд нулем и переходим к комментарию
                    comment = token;
                } else {
                    *token++ = '\0';  // Просто завершаем операнд
                    while (*token && isspace(*token)) token++;
                    comment = (*token == '*' || *token == ';') ? token + 1 : "";  // Проверяем, начинается ли комментарий
                }
            } else {
                comment = "";
            }
        }

        // Добавляем запись в таблицу
        add_entry(line_table, label, mnemonic, operand, comment);
    }
}
