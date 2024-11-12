#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "hash.h"
#include "macro.h"

HashTableMnemo* initializeMnemoTable() {
    int tableSize = 64; // Задаем размер хеш-таблицы
    HashTableMnemo* mnemoTable = createHashTableMnemo(tableSize);

    // Вставляем стандартные мнемоники MIX в таблицу
    insertMnemo(mnemoTable, "NOP", 0, 0, 0);
    insertMnemo(mnemoTable, "ADD", 0, 0, 1);
    insertMnemo(mnemoTable, "SUB", 0, 0, 2);
    insertMnemo(mnemoTable, "MUL", 0, 0, 3);
    insertMnemo(mnemoTable, "DIV", 0, 0, 4);
    insertMnemo(mnemoTable, "NUM", 0, 0, 5);
    insertMnemo(mnemoTable, "CHAR", 0, 0, 5);
    insertMnemo(mnemoTable, "HLT", 0, 0, 5);
    insertMnemo(mnemoTable, "SLA", 0, 0, 6);
    insertMnemo(mnemoTable, "SRA", 0, 1, 6);
    insertMnemo(mnemoTable, "SLAX", 0, 2, 6);
    insertMnemo(mnemoTable, "SRAX", 0, 3, 6);
    insertMnemo(mnemoTable, "SLC", 0, 4, 6);
    insertMnemo(mnemoTable, "SRC", 0, 5, 6);
    insertMnemo(mnemoTable, "MOVE", 0, 0, 7);
    insertMnemo(mnemoTable, "LDA", 0, 0, 8);
    insertMnemo(mnemoTable, "LD1", 0, 0, 9);
    insertMnemo(mnemoTable, "LD2", 0, 0, 10);
    insertMnemo(mnemoTable, "LD3", 0, 0, 11);
    insertMnemo(mnemoTable, "LD4", 0, 0, 12);
    insertMnemo(mnemoTable, "LD5", 0, 0, 13);
    insertMnemo(mnemoTable, "LD6", 0, 0, 14);
    insertMnemo(mnemoTable, "LDX", 0, 0, 15);
    insertMnemo(mnemoTable, "LDAN", 0, 0, 16);
    insertMnemo(mnemoTable, "LD1N", 0, 0, 17);
    insertMnemo(mnemoTable, "LD2N", 0, 0, 18);
    insertMnemo(mnemoTable, "LD3N", 0, 0, 19);
    insertMnemo(mnemoTable, "LD4N", 0, 0, 20);
    insertMnemo(mnemoTable, "LD5N", 0, 0, 21);
    insertMnemo(mnemoTable, "LD6N", 0, 0, 22);
    insertMnemo(mnemoTable, "LDXN", 0, 0, 23);
    insertMnemo(mnemoTable, "STA", 0, 0, 24);
    insertMnemo(mnemoTable, "ST1", 0, 0, 25);
    insertMnemo(mnemoTable, "ST2", 0, 0, 26);
    insertMnemo(mnemoTable, "ST3", 0, 0, 27);
    insertMnemo(mnemoTable, "ST4", 0, 0, 28);
    insertMnemo(mnemoTable, "ST5", 0, 0, 29);
    insertMnemo(mnemoTable, "ST6", 0, 0, 30);
    insertMnemo(mnemoTable, "STX", 0, 0, 31);
    insertMnemo(mnemoTable, "STJ", 0, 0, 32);
    insertMnemo(mnemoTable, "STZ", 0, 0, 33);
    insertMnemo(mnemoTable, "JMP", 0, 0, 39);
    insertMnemo(mnemoTable, "JSJ", 0, 1, 39);
    insertMnemo(mnemoTable, "JOV", 0, 2, 39);
    insertMnemo(mnemoTable, "JNOV", 0, 3, 39);
    insertMnemo(mnemoTable, "JL", 0, 4, 39);
    insertMnemo(mnemoTable, "JE", 0, 5, 39);
    insertMnemo(mnemoTable, "JG", 0, 6, 39);
    insertMnemo(mnemoTable, "JGE", 0, 7, 39);
    insertMnemo(mnemoTable, "JNE", 0, 8, 39);
    insertMnemo(mnemoTable, "JLE", 0, 9, 39);
    insertMnemo(mnemoTable, "IN", 0, 0, 36);
    insertMnemo(mnemoTable, "OUT", 0, 0, 37);
    insertMnemo(mnemoTable, "IOC", 0, 0, 35);

    return mnemoTable;
}

int main() {
    FILE* in = fopen("../in.txt", "r");
    FILE* listing = fopen("../files/listing.txt", "w");
    if (listing == NULL) {
        perror("Не удалось открыть файл listing.txt");
        return EXIT_FAILURE;
    }
    FILE* bin_dump = fopen("../files/bin_dump.txt", "w");
    if (bin_dump == NULL) {
        perror("Не удалось открыть файл bin_dump.txt");
        fclose(listing);
        return EXIT_FAILURE;
    }
    FILE* hex_dump = fopen("../files/hex_dump.txt", "w");
    if (hex_dump == NULL) {
        perror("Не удалось открыть файл hex_dump.txt");
        fclose(listing);
        fclose(bin_dump);
        return EXIT_FAILURE;
    }

    fprintf(listing, "LISTING\n\nPLACE       PROGRAMME          opcode   ADDRESS      CODE\n");
    fprintf(bin_dump, "BIN_DUMP\n\n");
    fprintf(hex_dump, "HEX_DUMP\n\n");

    line_t *a = create_line_t();
    if (a == NULL) {
        perror("Не удалось создать таблицу строк");
        fclose(listing);
        fclose(bin_dump);
        fclose(hex_dump);
        return EXIT_FAILURE;
    }
    macro_process(in);
    FILE* out = fopen("../files/output.txt", "r");
    get_line_t(out, a);
    HashTableNames *names = createHashTableNames(50);
    if (names == NULL) {
        perror("Не удалось создать таблицу имен");
        freeLineTable(a);
        fclose(listing);
        fclose(bin_dump);
        fclose(hex_dump);
        return EXIT_FAILURE;
    }

    HashTableMnemo *mnemo = initializeMnemoTable();
    if (mnemo == NULL) {
        perror("Не удалось создать таблицу мнемоник");
        freeHashTableNames(names);
        freeLineTable(a);
        fclose(listing);
        fclose(bin_dump);
        fclose(hex_dump);
        return EXIT_FAILURE;
    }

    int* adresses = (int*)malloc((a->size + 1) * sizeof(int));
    if (adresses == NULL) {
        perror("Не удалось выделить память для массива адресов");
        freeHashTableMnemo(mnemo);
        freeHashTableNames(names);
        freeLineTable(a);
        fclose(listing);
        fclose(bin_dump);
        fclose(hex_dump);
        return EXIT_FAILURE;
    }
    adresses[0] = 0;
    int start_address = 0, end_address = 0;

    for (int i = 0; i < a->size; i++) {
        if (strcmp(a->entries[i]->mnemonic, "ORIG") == 0) {
            adresses[i + 1] = strtol(a->entries[i]->operand, NULL, 16);
            start_address = adresses[i + 1];
        } else if (strcmp(a->entries[i]->mnemonic, "END") == 0) {
            adresses[i + 1] = adresses[i] + 4;
            end_address = adresses[i + 1];
        } else if (!strcmp(a->entries[i]->mnemonic, "CON")||!(strcmp(a->entries[i]->mnemonic, "ALF"))||!(strcmp(a->entries[i]->mnemonic, "EQU"))) {
            adresses[i + 1] = adresses[i] + 3;
            insertNames(names, a->entries[i]->label, adresses[i] + 3, 1, 0);
        } else if (strcmp(a->entries[i]->label, "")) {
            insertNames(names, a->entries[i]->label, adresses[i], 1, 0);
            adresses[i + 1] = adresses[i] + 3;
        } else {
            adresses[i + 1] = adresses[i] + 3;
        }
    }

    for (int i = 0; i < a->size; i++) {
        int k = 0;
        if (!strcmp(a->entries[i]->mnemonic, "ORIG") || !strcmp(a->entries[i]->mnemonic, "END") ||
            !strcmp(a->entries[i]->mnemonic, "CON") || !strcmp(a->entries[i]->mnemonic, "ALF") ||
            !strcmp(a->entries[i]->mnemonic, "EQU")) k = 1;

        if (!k) {
            HashEntryMnemo *mn = searchMnemo(mnemo, a->entries[i]->mnemonic);
            HashEntryNames *nm = searchNames(names, a->entries[i]->operand);
            if (mn && nm) {
                fprintf(listing, " %04x %6s %6s %6s \t\t%02x \t\t%04x \t%02x%04x\n", adresses[i], a->entries[i]->label, a->entries[i]->mnemonic, a->entries[i]->operand, mn->opcode, nm->address, mn->opcode, nm->address);
                fprintf(bin_dump, "%02x%04x", mn->opcode, nm->address);
                fprintf(hex_dump, ":03%04x00%02x%04xXX\n", adresses[i], mn->opcode, nm->address);
            }
        }
        else if (!strcmp(a->entries[i]->mnemonic, "HLT")) {
            HashEntryMnemo *mn = searchMnemo(mnemo, a->entries[i]->mnemonic);
            if (mn) {
                fprintf(listing, " %04x %6s %6s %6s \t\t%02x \t\t%04x \t%02x%04x\n", adresses[i], a->entries[i]->label, a->entries[i]->mnemonic, a->entries[i]->operand, mn->opcode, 0, mn->opcode, 0);
                fprintf(bin_dump, "%02x%04x", mn->opcode, 0);
                fprintf(hex_dump, ":03%04x00%02x%04xXX\n", adresses[i], mn->opcode, 0);
            }
        }
        else if (!strcmp(a->entries[i]->mnemonic, "END")) {
            fprintf(listing, " %04x %6s %6s %6s\n", adresses[i], a->entries[i]->label, a->entries[i]->mnemonic, a->entries[i]->operand);
            fprintf(hex_dump, ":00%04x01XX", start_address);
        }
        else if (!strcmp(a->entries[i]->mnemonic, "START")) {
            fprintf(listing, " %04x %6s %6s %6s\n", adresses[i], a->entries[i]->label, a->entries[i]->mnemonic, a->entries[i]->operand);
            fprintf(hex_dump, ":02%04x02%04xXX\n", start_address, end_address-start_address);
        }
        else {
            fprintf(listing, " %04x %6s %6s %6s \t\t%2s \t\t%4s \t%06x\n", adresses[i], a->entries[i]->label, a->entries[i]->mnemonic,
                    a->entries[i]->operand, "", "", strtol(a->entries[i]->operand, NULL, 10));
            fprintf(bin_dump, "%06x", strtol(a->entries[i]->operand, NULL, 10));
            fprintf(hex_dump, ":03%04x00%06xXX\n", adresses[i], strtol(a->entries[i]->operand, NULL, 10));
        }
    }

    fprintf(listing, "\nSTART ADDRESS: %xh \tSIZE OF PROGRAMME: %xh\n", start_address, end_address - start_address);
    fprintf(listing, "\n\t\t\tNames table\n%12s %12s %12s\n", "NAME", "SIGH", "ADDRESS");
    printSortedHashTableNames(listing, names);
    fprintf(listing, "\n\t\t\tMnemonic table\n%12s %12s %12s\n", "MNEMO", "COMMAND/DIR", "opcode");
    printSortedHashTableMnemo(listing, mnemo);

    freeHashTableMnemo(mnemo);
    freeHashTableNames(names);
    free(adresses);
    freeLineTable(a);

    fclose(listing);
    fclose(bin_dump);
    fclose(hex_dump);
    fclose(in);
    fclose(out);

    return 0;
}
