#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
    authors:    iimashfaaq
                prachidhingra09
                jackedhawk-117
*/

// Function to reverse a string in place
void strreverse(char* begin, char* end) {
    char aux;
    // Swap characters from start and end until middle is reached
    while (end > begin) {
        aux = *end;
        *end-- = *begin;
        *begin++ = aux;
    }
}

// Function to convert an integer to a string with a specified base
void itoa(int value, char* str, int base) {
    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz"; // Base symbols
    char* wstr = str; // Pointer to write position
    int sign; // Variable to store the sign of the number
    div_t res; // Variable to store division result

    // Check if the base is valid (2 to 35)
    if (base < 2 || base > 35) { *wstr = '\0'; return; }

    // If value is negative, store the sign and make value positive
    if ((sign = value) < 0) value = -value;

    // Convert the number to the given base and store in reverse order
    do {
        res = div(value, base); // Divide and get quotient and remainder
        *wstr++ = num[res.rem]; // Store remainder character
    } while ((value = res.quot)); // Repeat until quotient is 0
    
    // Add minus sign if number was negative
    if (sign < 0) *wstr++ = '-';
    *wstr = '\0'; // Null-terminate the string

    // Reverse the string to get the correct order
    strreverse(str, wstr - 1);
}

int main() {
    // Declare variables for string storage and reading file contents
    char a[10], ad[10], label[10], opcode[10], operand[10], mnemonic[10], symbol[10];
    int i, address, sa, code, add, len, actual_len, tcount = 0;
    FILE *fp2, *fp3, *fp4, *fp5; // File pointers for file handling

    // Open the symbol table file and check for errors
    fp2 = fopen("symtab.txt", "r");
    if (!fp2) { perror("Failed to open symtab.txt"); return 1; }

    // Open the intermediate file and check for errors
    fp3 = fopen("intermediate.txt", "r");
    if (!fp3) { perror("Failed to open intermediate.txt"); fclose(fp2); return 1; }

    // Open the operation table file and check for errors
    fp4 = fopen("opTab.txt", "r");
    if (!fp4) { perror("Failed to open opTab.txt"); fclose(fp2); fclose(fp3); return 1; }

    // Open the output file for writing and check for errors
    fp5 = fopen("out.txt", "w");
    if (!fp5) { perror("Failed to open out.txt"); fclose(fp2); fclose(fp3); fclose(fp4); return 1; }

    // Read the first line from the intermediate file (label, opcode, operand)
    fscanf(fp3, "%s%s%s", label, opcode, operand);

    // If the opcode is "START", initialize the program start address
    if (strcmp(opcode, "START") == 0) {
        // Print the header record to the output file
        fprintf(fp5, "H***^%s^000023\n", operand); // "000023" is a placeholder for program length
        // Read next line with the address and other fields
        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
        sa = address; // Store the start address
    }

    // Start the text record with the start address in the output file
    fprintf(fp5, "T^%06d^", sa); // Text record start address in hexadecimal
    // Process each line until the "END" opcode is reached
    while (strcmp(opcode, "END") != 0) {
        // If the opcode is "BYTE"
        if (strcmp(opcode, "BYTE") == 0) {
            len = strlen(operand); // Length of the operand
            actual_len = len - 3; // Adjust for starting and ending delimiters
            for (i = 2; i < (actual_len + 2); i++) { // Process each character
                itoa(operand[i], ad, 16); // Convert character to hex
                fprintf(fp5, "%s", ad); // Write hex value to output
                tcount++; // Increment text record count
            }
        } 
        // If the opcode is "WORD"
        else if (strcmp(opcode, "WORD") == 0) {
            itoa(atoi(operand), a, 16); // Convert operand to integer, then to hex
            fprintf(fp5, "00000%s", a); // Write with padding to output file
            tcount += 3; // WORD occupies 3 bytes
        } 
        // If the opcode is "RESB" or "RESW", skip them in the object program
        else if ((strcmp(opcode, "RESB") == 0) || (strcmp(opcode, "RESW") == 0)) {
            // Do nothing, as these directives are for reserving space
        } 
        else { // Process general instructions
            rewind(fp4); // Go back to start of the opTab file
            fscanf(fp4, "%s%d", mnemonic, &code); // Read mnemonic and opcode
            // Find the matching opcode in opTab
            while (strcmp(opcode, mnemonic) != 0) {
                fscanf(fp4, "%s%d", mnemonic, &code); // Read next line
            }
            if (strcmp(operand, "-") == 0) { // If operand is empty
                fprintf(fp5, "%02d0000", code); // Write opcode with no address
                tcount += 3; // Increment count by 3 bytes
            } else {
                rewind(fp2); // Go back to start of symtab
                fscanf(fp2, "%s%d", symbol, &add); // Read symbol and address
                // Find the matching symbol in symtab
                while (strcmp(operand, symbol) != 0) {
                    fscanf(fp2, "%s%d", symbol, &add); // Read next line
                }
                fprintf(fp5, "%02d%04d", code, add); // Write opcode and address
                tcount += 3; // Increment count by 3 bytes
            }
        }

        // Read the next line from the intermediate file
        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
    }
    fprintf(fp5, "^%02d\n", tcount); // End of text record with count

    // Write the end record with the start address
    fprintf(fp5, "E^%06d\n", sa);
    printf("Object Program Generated\n"); // Print confirmation message

    // Close all open files
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);
    return 0; // Exit the program
}
