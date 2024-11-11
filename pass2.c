#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void strreverse(char* begin, char* end) {
    char aux;
    while (end > begin) {
        aux = *end;
        *end-- = *begin;
        *begin++ = aux;
    }
}

void itoa(int value, char* str, int base) {
    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* wstr = str;
    int sign;
    div_t res;

    // Validate base
    if (base < 2 || base > 35) { *wstr = '\0'; return; }

    // Take care of sign
    if ((sign = value) < 0) value = -value;

    // Conversion. Number is reversed.
    do {
        res = div(value, base);
        *wstr++ = num[res.rem];
    } while ((value = res.quot));
    
    if (sign < 0) *wstr++ = '-';
    *wstr = '\0';

    // Reverse string
    strreverse(str, wstr - 1);
}

int main() {
    char a[10], ad[10], label[10], opcode[10], operand[10], mnemonic[10], symbol[10];
    int i, address, sa, code, add, len, actual_len, tcount = 0;
    FILE *fp2, *fp3, *fp4, *fp5;

    // Open files and check for errors
    fp2 = fopen("symtab.txt", "r");
    if (!fp2) { perror("Failed to open symtab.txt"); return 1; }

    fp3 = fopen("intermediate.txt", "r");
    if (!fp3) { perror("Failed to open intermediate.txt"); fclose(fp2); return 1; }

    fp4 = fopen("opTab.txt", "r");
    if (!fp4) { perror("Failed to open opTab.txt"); fclose(fp2); fclose(fp3); return 1; }

    fp5 = fopen("out.txt", "w");
    if (!fp5) { perror("Failed to open out.txt"); fclose(fp2); fclose(fp3); fclose(fp4); return 1; }

    fscanf(fp3, "%s%s%s", label, opcode, operand);

    if (strcmp(opcode, "START") == 0) {
        // Print header record
        fprintf(fp5, "H***^%s^000023\n", operand); // Placeholder for length
        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
        sa = address;
    }

    fprintf(fp5, "T^%06d^", sa); // Initial Text record start address
    while (strcmp(opcode, "END") != 0) {
        if (strcmp(opcode, "BYTE") == 0) {
            len = strlen(operand);
            actual_len = len - 3;
            for (i = 2; i < (actual_len + 2); i++) {
                itoa(operand[i], ad, 16);
                fprintf(fp5, "%s", ad);
                tcount++;
            }
        } else if (strcmp(opcode, "WORD") == 0) {
            itoa(atoi(operand), a, 16);
            fprintf(fp5, "00000%s", a);
            tcount += 3;
        } else if ((strcmp(opcode, "RESB") == 0) || (strcmp(opcode, "RESW") == 0)) {
            // Skip RESB and RESW in object program
        } else {
            rewind(fp4);
            fscanf(fp4, "%s%d", mnemonic, &code);
            while (strcmp(opcode, mnemonic) != 0) {
                fscanf(fp4, "%s%d", mnemonic, &code);
            }
            if (strcmp(operand, "-") == 0) {
                fprintf(fp5, "%02d0000", code);
                tcount += 3;
            } else {
                rewind(fp2);
                fscanf(fp2, "%s%d", symbol, &add);
                while (strcmp(operand, symbol) != 0) {
                    fscanf(fp2, "%s%d", symbol, &add);
                }
                fprintf(fp5, "%02d%04d", code, add);
                tcount += 3;
            }
        }

        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
    }
    fprintf(fp5, "^%02d\n", tcount); // End of Text record with count

    // End record
    fprintf(fp5, "E^%06d\n", sa);
    printf("Object Program Generated\n");

    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);
    return 0;
}
