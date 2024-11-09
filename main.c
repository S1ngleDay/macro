#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "hash.h"

int main() {
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

    fprintf(listing, "LISTING\n\nPLACE       PROGRAMME          KOP   ADDRESS      CODE\n");
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
    get_line_t(a);

    HashTableNames *names = createHashTableNames(50);
    if (names == NULL) {
        perror("Не удалось создать таблицу имен");
        freeLineTable(a);
        fclose(listing);
        fclose(bin_dump);
        fclose(hex_dump);
        return EXIT_FAILURE;
    }

    HashTableMnemo *mnemo = createHashTableMnemo(50);
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

    insertMnemo(mnemo, "START", 0, 1, 0, 0);
    insertMnemo(mnemo, "END", 0, 1, 0, 0);
    insertMnemo(mnemo, "WORD", 0, 1, 0, 0);
    insertMnemo(mnemo, "RESW", 0, 1, 0, 0);
    insertMnemo(mnemo, "ADD", 0, 0, 0, 0x11);
    insertMnemo(mnemo, "ADDX", 0, 0, 0, 0x12);
    insertMnemo(mnemo, "LDA", 0, 0, 0, 0x21);
    insertMnemo(mnemo, "LDX", 0, 0, 0, 0x22);
    insertMnemo(mnemo, "STA", 0, 0, 0, 0x31);
    insertMnemo(mnemo, "HLT", 0, 0, 0, 0x0f);

    for (int i = 0; i < a->size; i++) {
        if (strcmp(a->entries[i]->mnemonic, "START") == 0) {
            adresses[i + 1] = strtol(a->entries[i]->operand, NULL, 16);
            start_address = adresses[i + 1];
        } else if (strcmp(a->entries[i]->mnemonic, "END") == 0) {
            adresses[i + 1] = adresses[i] + 4;
            end_address = adresses[i + 1];
        } else if (!strcmp(a->entries[i]->mnemonic, "WORD")) {
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
        if (!strcmp(a->entries[i]->mnemonic, "START") || !strcmp(a->entries[i]->mnemonic, "END") ||
            !strcmp(a->entries[i]->mnemonic, "WORD") || !strcmp(a->entries[i]->mnemonic, "RESB") ||
            !strcmp(a->entries[i]->mnemonic, "RESW") || !strcmp(a->entries[i]->mnemonic, "HLT")) k = 1;

        if (!k) {
            HashEntryMnemo *mn = searchMnemo(mnemo, a->entries[i]->mnemonic);
            HashEntryNames *nm = searchNames(names, a->entries[i]->operand);
            if (mn && nm) {
                fprintf(listing, " %04x %6s %6s %6s \t\t%02x \t\t%04x \t%02x%04x\n", adresses[i], a->entries[i]->label, a->entries[i]->mnemonic, a->entries[i]->operand, mn->KOP, nm->address, mn->KOP, nm->address);
                fprintf(bin_dump, "%02x%04x", mn->KOP, nm->address);
                fprintf(hex_dump, ":03%04x00%02x%04xXX\n", adresses[i], mn->KOP, nm->address);
            }
        }
        else if (!strcmp(a->entries[i]->mnemonic, "HLT")) {
            HashEntryMnemo *mn = searchMnemo(mnemo, a->entries[i]->mnemonic);
            if (mn) {
                fprintf(listing, " %04x %6s %6s %6s \t\t%02x \t\t%04x \t%02x%04x\n", adresses[i], a->entries[i]->label, a->entries[i]->mnemonic, a->entries[i]->operand, mn->KOP, 0, mn->KOP, 0);
                fprintf(bin_dump, "%02x%04x", mn->KOP, 0);
                fprintf(hex_dump, ":03%04x00%02x%04xXX\n", adresses[i], mn->KOP, 0);
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
    fprintf(listing, "\n\t\t\tMnemonic table\n%12s %12s %12s\n", "MNEMO", "COMMAND/DIR", "KOP");
    printSortedHashTableMnemo(listing, mnemo);

    freeHashTableMnemo(mnemo);
    freeHashTableNames(names);
    free(adresses);
    freeLineTable(a);

    fclose(listing);
    fclose(bin_dump);
    fclose(hex_dump);

    return 0;
}
