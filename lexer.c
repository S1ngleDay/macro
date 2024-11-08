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

void add_entry(line_t *table, char *label, char *mnemonic, char *operand, char *comment) {
    line_e *entry = (line_e *)malloc(sizeof(line_e));
    if (entry == NULL) {
        perror("Failed to allocate memory for line_e");
        exit(EXIT_FAILURE);
    }
    entry->label = strdup(label);
    entry->mnemonic = strdup(mnemonic);
    entry->operand = strdup(operand);
    entry->comment = strdup(comment);

    table->entries = (line_e **)realloc(table->entries, sizeof(line_e *) * (table->size + 1));
    if (table->entries == NULL) {
        perror("Failed to allocate memory for entries");
        exit(EXIT_FAILURE);
    }
    table->entries[table->size] = entry;
    table->size++;
}

void get_line_t(line_t *line_table) {
    char *arr[4] = {NULL, NULL, NULL, NULL};  // Для хранения label, mnemonic, operand, comment

    char input_string[MAX_LINE_LENGTH];
    char *token;

    FILE *in = fopen("../in.txt", "r");
    if (in == NULL) {
        perror("Не удалось открыть файл");
        return;
    }

    while (fgets(input_string, MAX_LINE_LENGTH, in) != NULL) {
        input_string[strcspn(input_string, "\n")] = '\0';  // Удаляем символ новой строки

        // Пропускаем пустые строки и строки, состоящие только из пробелов
        if (input_string[0] == '\0' || strspn(input_string, " \t") == strlen(input_string)) {
            continue;
        }

        // Если строка начинается с '*', то вся строка - это комментарий
        if (input_string[0] == '*') {
            arr[3] = input_string + 1;  // Сохраняем комментарий, пропуская символ '*'
            arr[0] = arr[1] = arr[2] = "";  // Метка, мнемоника и операнд пустые
        } else {
            char *rest = input_string;

            // Разделяем строку на токены
            if (isspace(input_string[0])) {
                // Если строка начинается с пробела или табуляции, то это мнемоника
                token = strtok_r(rest, " \t", &rest);
            } else {
                token = strtok_r(rest, " \t", &rest);
                if (token && isalpha(token[0])) {
                    arr[0] = token;  // Метка
                    token = strtok_r(NULL, " \t", &rest);  // Следующий токен - мнемоника
                }
            }

            if (token != NULL) {
                arr[1] = token;  // Мнемоника
                token = strtok_r(NULL, " \t", &rest);  // Следующий токен - операнд
            }

            if (token != NULL) {
                arr[2] = token;  // Операнд
                // Обработка комментария, если он есть
                if (rest != NULL) {
                    while (*rest == ' ' || *rest == '\t') {  // Пропускаем пробелы
                        rest++;
                    }
                    if (*rest != '\0') {
                        arr[3] = rest;  // Комментарий
                    }
                } else arr[3] = "";
            }
        }

        // Добавляем запись в таблицу, проверяя, что все данные корректны
        if (arr[0] == NULL) arr[0] = "";  // Убедимся, что метка существует
        if (arr[1] == NULL) arr[1] = "";  // Убедимся, что мнемоника существует
        if (arr[2] == NULL) arr[2] = "";  // Убедимся, что операнд существует
        if (arr[3] == NULL) arr[3] = "";  // Убедимся, что комментарий существует


        add_entry(line_table, arr[0], arr[1], arr[2], arr[3]);
    }

    fclose(in);
}
