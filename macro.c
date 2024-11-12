#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"

#define TABLE_SIZE 100
#define NAME_SIZE 20
#define BUFF_SIZE 256

// Создание нового макроса в таблице
Macro* create_new_macro(int pos, char* buffer, unsigned start, char* str) {
    Macro* newmacro = (Macro*)malloc(sizeof(Macro));
    newmacro->param_count = 0;
    newmacro->start_index = start;
    strcpy(newmacro->name, strtok(buffer, " "));
    newmacro->name[NAME_SIZE - 1] = '\0';
    char* params = str + strlen("macro");
    if (strcmp("\n", params) != 0) {
        char* param = strtok(params, " ,\n");
        for (int i = 0; param; i++) {
            newmacro->params[i] = (char*)malloc(BUFF_SIZE);
            if (!newmacro->params[i]) {
                perror("Ошибка выделения памяти для параметра");
                exit(EXIT_FAILURE);
            }
            strncpy(newmacro->params[i], param, BUFF_SIZE - 1);
            newmacro->params[i][BUFF_SIZE - 1] = '\0';
            newmacro->param_count++;
            param = strtok(NULL, " ,\n");
        }
    }
    return newmacro;
}

// Замена подстроки в строке
void replace_string(char *string, char *from, char *to) {
    char *p = strstr(string, from);
    if (p != NULL) {
        int len_from = strlen(from);
        int len_to = strlen(to);
        memmove(p + len_to, p + len_from, strlen(p + len_from) + 1);
        strncpy(p, to, len_to);
    }
}

void replace_macro(char strings[][BUFF_SIZE], int stringsCount, Macro* macro, char *call_line, FILE *output_file) {
    // Разбираем строку вызова макроса
    strtok(call_line, " \t"); //отрезаем вызов макроопределения, оставляя параметры
    char *params[TABLE_SIZE] = {NULL}; //массив фактических параметров
    char *param = strtok(NULL, ",\n"); //текущий формальный параметр
    for (int i = 0; param; i++) {
        params[i] = param;
        param = strtok(NULL, ",\n");
    }
    // Подставляем строки макроопределения
    for (int j = macro->start_index; j < macro->end_index; j++) {
        char line[BUFF_SIZE];
        strcpy(line, strtok(strings[j], " "));
        char line2[BUFF_SIZE];
        strcpy(line2, strtok(NULL, "\n"));
        // Заменяем формальные параметры на фактические
        for (int k = 0; k < macro->param_count; k++) if (params[k] != NULL) replace_string(line2, macro->params[k], params[k]);
        fprintf(output_file, "\t%s%s\n", line, line2);
    }
}

// Обработка макросов
void macro_process(FILE* in) {
    Macro* macros[TABLE_SIZE] = {NULL};
    char strings[TABLE_SIZE][BUFF_SIZE] = {{0}};
    unsigned stringsCount = 0;
    unsigned macro_count = 0;
    unsigned macro_call_count = 0;
    int macro_writing = 0;
    char* str;
    char buffer[BUFF_SIZE];

    FILE* out = fopen("../files/output.txt", "w");
    if (!out) {
        perror("Ошибка открытия выходного файла");
        return;  // Завершаем функцию, если не удалось открыть файл
    }

    while (fgets(buffer, BUFF_SIZE, in)) {
        if (macro_writing) {
            if (strstr(buffer, "ENDM")) {
                macro_call_count--;
                if (macro_call_count == 0) {
                    macros[macro_count]->end_index = stringsCount;
                    macro_writing = 0;
                    macro_count++;
                }
            } else {
                str = strstr(buffer, "MACRO");
                if (str) macro_call_count++;
                strncpy(strings[stringsCount], buffer, BUFF_SIZE - 1);
                strings[stringsCount][BUFF_SIZE - 1] = '\0';
                stringsCount++;
            }
        } else {
            char* str = strstr(buffer, "MACRO");
            if (str) {
                macro_writing = 1;
                macros[macro_count] = create_new_macro(macro_count, buffer, stringsCount, str);
                macro_call_count++;
            } else {
                int macro_call = 0;
                for (int i = 0; i < macro_count; i++) {
                    if (strstr(buffer, macros[i]->name)) {
                        replace_macro(strings, stringsCount, macros[i], buffer, out);
                        macro_call = 1;
                        break;
                    }
                }
                if (!macro_call && strcmp(buffer, "\n")) fprintf(out, "%s", buffer);
            }
        }
    }

    fclose(out);

    // Очистка памяти
    for (int i = 0; i < macro_count; i++) {
        for (int j = 0; j < macros[i]->param_count; j++) free(macros[i]->params[j]);
        free(macros[i]);
    }
}
