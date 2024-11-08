#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256

typedef struct {
    char label[32];
    char mnemonic[16];
    char operand[128];
    char comment[128];
} MixalLine;

int main() {
    MixalLine parsed;  // Объявляем структуру как обычную переменную
    char input_string[MAX_LINE_LENGTH];
    char *token;

    FILE *in = fopen("../in.txt", "r");
    if (in == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }

    // Чтение строки из файла
    while (fgets(input_string, MAX_LINE_LENGTH, in) != NULL) {
        // Удаляем символ новой строки, если он есть
        input_string[strcspn(input_string, "\n")] = '\0';

        // Пропускаем пустые строки и строки, состоящие только из пробелов
        if (input_string[0] == '\0' || strspn(input_string, " \t") == strlen(input_string)) {
            continue;
        }

        // Очищаем структуру перед парсингом новой строки
        memset(&parsed, 0, sizeof(MixalLine));

        // Проверяем, начинается ли строка с символа '*'
        if (input_string[0] == '*') {
            // Сохраняем всю строку как комментарий
            strncpy(parsed.comment, input_string + 1, sizeof(parsed.comment) - 1);  // Пропускаем '*'
        } else {
            char *rest = input_string;

            // Проверяем, начинается ли строка с пробела или табуляции
            if (isspace(input_string[0])) {
                // Если строка начинается с пробела или табуляции, значит, нет метки
                token = strtok_r(rest, " \t", &rest);  // Сразу парсим мнемоник
            } else {
                // Если строка не начинается с пробела, то первый токен может быть меткой
                token = strtok_r(rest, " \t", &rest);
                if (token && isalpha(token[0])) {
                    strncpy(parsed.label, token, sizeof(parsed.label) - 1);
                    token = strtok_r(NULL, " \t", &rest);  // Следующий токен - мнемоник
                }
            }

            // Парсинг мнемоника
            if (token != NULL) {
                strncpy(parsed.mnemonic, token, sizeof(parsed.mnemonic) - 1);
                token = strtok_r(NULL, " \t", &rest);  // Получаем следующий токен
            }

            // Парсинг операнда и комментария
            if (token != NULL) {
                strncpy(parsed.operand, token, sizeof(parsed.operand) - 1);

                // Проверка на наличие комментария после операнда
                if (rest != NULL) {
                    // Удаляем начальные пробелы у комментария
                    while (*rest == ' ' || *rest == '\t') {
                        rest++;
                    }
                    // Сохраняем комментарий, если что-то осталось в rest
                    if (*rest != '\0') {
                        strncpy(parsed.comment, rest, sizeof(parsed.comment) - 1);
                    }
                }
            }
        }

        // Вывод результатов
        printf("label: %s | mnemo: %s | oper: %s | comment: %s\n",
               parsed.label, parsed.mnemonic, parsed.operand, parsed.comment);
    }

    fclose(in);
    return 0;
}
