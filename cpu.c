/*
 * Dumb8 8-bit processing unit
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
#include "cpu.h"

CPU_t cpu;

/*
 * Initializing the CPU: program counter and registers to zero, halted flag to false.
*/

void cpu_init()
{
	printf("Initializing Dumb8 CPU...\n");
	cpu.pc = 0;
	cpu.halted = false;
	cpu.equal_flag = false;
	cpu.flag_clear_delay = 0;
	for (size_t i=0; i<NUM_REGISTERS; i++)
	{
		cpu.reg[i] = 0;
	}
}

/*
 * Executing an instruction: this is the equivalent of the ALU in a higher-level point of view
*/

void cpu_exec(uint8_t opcode)
{
	uint8_t reg1, reg2, addr, value;

	if (cpu.flag_clear_delay > 0)
	{
		cpu.flag_clear_delay--;
		if (cpu.flag_clear_delay == 0)
		{
			cpu.equal_flag = false;
		}
	}

	switch (opcode)
	{
		// Some lines are repeating.. Should make this a better way..
		case NOP:
			break;
		case MOV:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] = cpu.reg[reg2];
			break;
		case PUT:
			reg1 = cpu.memory[cpu.pc++];
			value = cpu.memory[cpu.pc++];
			cpu.reg[reg1] = value;
			break;
		case ADD:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] += cpu.reg[reg2];
			break;
		case SUB:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] -= cpu.reg[reg2];
			break;
		case MUL:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] *= cpu.reg[reg2];
			break;
		case DIV:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] /= cpu.reg[reg2];
			break;
		case OR:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] |= cpu.reg[reg2];
			break;
		case AND:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] &= cpu.reg[reg2];
			break;
		case XOR:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] ^= cpu.reg[reg2];
			break;
		case HLT:
			cpu.halted = true;
			break;
		case JMP:
			addr = cpu.memory[cpu.pc++];
			cpu.pc = addr;
			break;
		case JEQ:
			reg1 = cpu.memory[cpu.pc++];
			addr = cpu.memory[cpu.pc++];
			if (cpu.equal_flag) {
				cpu.pc = addr;
			}
			break;
		case CMP:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.equal_flag = (cpu.reg[reg1] == cpu.reg[reg2]);
			cpu.flag_clear_delay = 2;
			break;
		case OUT:
			reg1 = cpu.memory[cpu.pc++];
			putchar(cpu.reg[reg1]);
			break;
		case IN:
			reg1 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] = getchar();
			break;
		default:
			printf("Unknown instruction: 0x%02X\n", opcode);
			cpu.halted = true;
			break;
	}
}

/*
 * Loading the program in memory 
*/

void cpu_load(const uint8_t* program, size_t size)
{
	for (size_t i=0; i<size; i++)
	{
		cpu.memory[i] = program[i];
	}
}

void load_program_from_bin(char* binary_file)
{
	FILE* binary_fp = fopen(binary_file, "rb");

	if (!binary_fp)
	{
		printf("Cannot open file '%s' for reading.\n", binary_file);
		exit(1);
	}

	fseek(binary_fp, 0, SEEK_END);
    size_t size = ftell(binary_fp);
    rewind(binary_fp);

	uint8_t* program_buffer = (uint8_t*)malloc(size);
    if (!program_buffer)
    {
        printf("Memory allocation failed\n");
        fclose(binary_fp);
        exit(1);
    }

	fread(program_buffer, sizeof(uint8_t), size, binary_fp);
	cpu_load(program_buffer, size);
	free(program_buffer);
	fclose(binary_fp);
}

/*
 * Running the program, incrementing the program counter for each instruction ran
*/

void cpu_run()
{
	printf("\n[BEGIN CPU OUTPUT]\n");
	while (!cpu.halted)
	{
		uint8_t opcode = cpu.memory[cpu.pc++];
		cpu_exec(opcode);
	}
	printf("\n[END CPU OUTPUT]\n");
}

/*
 * Dumping the CPU information, registers and flags
*/

void cpu_dump()
{
	printf("\n*** CPU state dump ***\nPC: 0x%x\nEqual flag: %d\nHalted: %d\n\n", cpu.pc, cpu.equal_flag, cpu.halted);
	
	for (size_t i=0; i<NUM_REGISTERS; i++)
	{
		printf("R%lu: 0x%x\n", i, cpu.reg[i]);
	}
	puts("");
}

/*
 * Dumping the memory contents
*/

void mem_dump()
{
	printf("\n*** Memory dump ***\n");
	for (size_t i=0; i<MEM_SIZE; i++)
	{
		if (i%20 == 0)
		{
			puts("");
		}

		switch (cpu.memory[i])
		{
			// Instructions (colored background)
			case 0xa0:
			case 0xa1:
				printf("\e[42m%02x\e[0m ", cpu.memory[i]);
				break;
		
			case 0xb0:
			case 0xb1:
			case 0xb2:
			case 0xb3:
				printf("\e[43m%02x\e[0m ", cpu.memory[i]);
				break;

			case 0xc0:
			case 0xc1:
			case 0xc2:
				printf("\e[45m%02x\e[0m ", cpu.memory[i]);
				break;
			
			case 0xd0:
				printf("\e[46m%02x\e[0m ", cpu.memory[i]);
				break;

			case 0xe0:
			case 0xe1:
			case 0xe2:	
				printf("\e[44m%02x\e[0m ", cpu.memory[i]);
				break;

			case 0xff:
				printf("\e[41m%02x\e[0m ", cpu.memory[i]);
				break;

			// General purpose registers (colored foreground)
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
				printf("\e[36m%02x\e[0m ", cpu.memory[i]);
				break;
			default:
				printf("%02x ", cpu.memory[i]);	
				break;
		}
	}
	puts("");
}

/*
 * Write to register: useful for debugging and testing
*/

void reg_write(int index, uint8_t val)
{
	cpu.reg[index] = val;
}

int main(int argc, char* argv[])
{
	cpu_init();

	if (argc < 2)
	{
		printf("Usage: %s <program>\n", argv[0]);
		return -1;
	}

	load_program_from_bin(argv[1]);

	// Dumping our program
	mem_dump();
	cpu_run();
	
	// Post-mortem analysis
	cpu_dump();

	return 0;
}
