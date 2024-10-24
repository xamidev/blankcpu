/*
 * Dumb8 code assembler
 * Made by github.com/xamidev
 *
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to <http://unlicense.org/>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../cpu.h"

/*
 * Reading the assembly file and writing its instructions in
 * opcode format in a binary file
*/

// The code is not optimized at all. But remember: it is a DUMB assembler!

void assemble(char* filename)
{
	FILE* fp = fopen(filename, "r");

	if (!fp)
	{
		printf("Cannot read file '%s'\n", filename);
		exit(1);
	}

	char* binary_file = strtok(filename, ".");
	binary_file = strcat(binary_file, ".bin");

	FILE* bin_fp = fopen(binary_file, "wb");

	if (!bin_fp)
	{
		printf("Cannot open file '%s' for writing.\n", binary_file);
		fclose(fp);
		exit(1);
	}

	char line[256] = {0};
	uint8_t buffer[BUF_MAX] = {0};
	size_t i = 0;

	while (fgets(line, sizeof(line), fp))
	{
		char instruction[10] = {0};
		char reg1[10] = {0};
		char reg2[10] = {0};
		int addr = 0;
		int value = 0;

		if (strncmp(line, ";", 1) == 0)
		{
			continue;
		}
		else if (strncmp(line, "\n", 1) == 0)
		{
			continue;
		}
		else if (strncmp(line, "HLT", 3) == 0)
		{
			buffer[i++] = HLT;
		}
		else if (strncmp(line, "NOP", 3) == 0)
		{
			buffer[i++] = NOP;
		}
		else if (strncmp(line, "MOV", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = MOV;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "ADD", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = ADD;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "CMP", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = CMP;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "SUB", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = SUB;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "MUL", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = MUL;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "DIV", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = DIV;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "OR", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = OR;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "AND", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = AND;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "XOR", 3) == 0)
		{
			sscanf(line, "%s %[^,], %s", instruction, reg1, reg2);	
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';
			buffer[i++] = XOR;
			buffer[i++] = reg1_n;
			buffer[i++] = reg2_n;
		}
		else if (strncmp(line, "JEQ", 3) == 0)
		{
			sscanf(line, "%s %d", instruction, &addr);
			buffer[i++] = JEQ;
			buffer[i++] = addr;
		}
		else if (strncmp(line, "JMP", 3) == 0)
		{
			sscanf(line, "%s %d", instruction, &addr);	
			buffer[i++] = JMP;
			buffer[i++] = addr;
		}
		else if (strncmp(line, "OUT", 3) == 0)
		{
			sscanf(line, "%s %s", instruction, reg1);
			int reg1_n = reg1[1] - '0';
			buffer[i++] = OUT;
			buffer[i++] = reg1_n;
		}
		else if (strncmp(line, "IN", 2) == 0)
		{
			sscanf(line, "%s %s", instruction, reg1);
			int reg1_n = reg1[1] - '0';
			buffer[i++] = IN;
			buffer[i++] = reg1_n;
		}
		else if (strncmp(line, "PUT", 3) == 0)
		{
			sscanf(line, "%s %[^,], %d", instruction, reg1, &value);
			int reg1_n = reg1[1] - '0';
			buffer[i++] = PUT;
			buffer[i++] = reg1_n;
			buffer[i++] = value;
		}
		else {
			printf("Unknown instruction '%s'\n", line);
		}
	}

	fwrite(buffer, sizeof(uint8_t), i, bin_fp);

	fclose(fp);
	fclose(bin_fp);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s <assembly file>\n", argv[0]);
		exit(1);
	}
	assemble(argv[1]);
	return 0;
}
