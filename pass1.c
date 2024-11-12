#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*
    authors:    iimashfaaq
                prachidhingra09
                jackedhawk-117
*/

int main() {
    // Variables to store different assembly components and addresses
    char opcode[10], mnemonic[3], operand[10], label[10], code[10];
    int locctr, start, length;
    
    // File pointers to handle input and output files
    FILE *fp1, *fp2, *fp3, *fp4, *fp5;  // Add fp5 for length.txt
    
    // Open files: input, symbol table, intermediate, opcode table, and length output
    fp1 = fopen("input.txt", "r");     // Assembly program input
    fp2 = fopen("symtab.txt", "w");    // Symbol table output
    fp3 = fopen("intermediate.txt", "w"); // Intermediate file output
    fp4 = fopen("opTab.txt", "r");     // Opcode table
    fp5 = fopen("length.txt", "w");    // Length file output

    // Read the first line of input (expected to contain the START directive)
    fscanf(fp1, "%s%s%s", label, opcode, operand);

    // If the first opcode is "START"
    if (strcmp(opcode, "START") == 0) {
        start = atoi(operand);  // Convert starting address from string to integer
        locctr = start;         // Initialize location counter with the starting address

        // Write START line to intermediate file
        fprintf(fp3, "%s\t%s\t%s\n", label, opcode, operand);
        
        // Read the next line from the input file
        fscanf(fp1, "%s%s%s", label, opcode, operand);
    } else {
        // If there's no START directive, initialize locctr to 0
        locctr = 0;
    }

    // Loop until "END" directive is encountered
    while (strcmp(opcode, "END") != 0) {
        // Write the current locctr value to the intermediate file
        fprintf(fp3, "%d\t", locctr);

        // If there's a label (not "-"), add it to the symbol table with its locctr
        if (strcmp(label, "-") != 0)
            fprintf(fp2, "%s\t%d\n", label, locctr);

        // Rewind the opcode table to the beginning
        rewind(fp4);
        
        // Read the first opcode from the opcode table
        fscanf(fp4, "%s", code);
        
        // Search for the opcode in the opcode table
        while (strcmp(code, "END") != 0) { // Stop if "END" of opTab is reached
            if (strcmp(opcode, code) == 0) {  // If opcode is found
                locctr += 3;  // Increment locctr by 3 bytes for standard instruction
                break;
            }
            fscanf(fp4, "%s", code);  // Read the next opcode in the table
        }

        // Handle specific directives: WORD, RESW, RESB, BYTE
        if (strcmp(opcode, "WORD") == 0)
            locctr += 3;  // WORD occupies 3 bytes
        else if (strcmp(opcode, "RESW") == 0)
            locctr += (3 * atoi(operand));  // RESW reserves multiple words
        else if (strcmp(opcode, "RESB") == 0)
            locctr += atoi(operand);  // RESB reserves specified number of bytes
        else if (strcmp(opcode, "BYTE") == 0)
            ++locctr;  // BYTE usually reserves 1 byte

        // Write label, opcode, and operand to intermediate file
        fprintf(fp3, "%s\t%s\t%s\n", label, opcode, operand);
        
        // Read the next line from the input file
        fscanf(fp1, "%s%s%s", label, opcode, operand);
    }

    // Final line after encountering the "END" directive
    fprintf(fp3, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);
    
    // Calculate the length of the program by subtracting the start address from the final locctr
    length = locctr - start;
    printf("The length of the program is %d", length);  // Output the program length

    // Write the length to length.txt
    fprintf(fp5, "%d", length);

    // Close all opened files
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);  // Close the length file

    getchar();  // Wait for user input before closing (useful in some environments)
    return 0;
} // store length to length.txt
