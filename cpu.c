/*
 * Unnamed 8-bit processing unit
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

#define MEM_SIZE	256
#define NUM_REGISTERS	4

/*
 * Instruction set
 * Here, we're making a RISC (reduced instruction set computer)
 * so we're staying minimalistic.
*/

typedef enum
{
	MOV = 1,
	ADD,
	SUB,
	LOAD,
	STORE,
	JMP,
	JEQ,
	HLT = 0xFF
} instruction_set_t;

/*
 * CPU structure definition
 * Contains 4 8-bit registers, memory, a program counter, and a halt switch.
*/

typedef struct
{
	uint8_t reg[NUM_REGISTERS];
	uint8_t memory[MEM_SIZE];
	uint16_t pc;
	bool halted;
} CPU_t;

CPU_t cpu;

/*
 * Initializing the CPU: program counter and registers to zero, halted flag to false.
*/

void cpu_init()
{
	cpu.pc = 0;
	cpu.halted = false;
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
	uint8_t reg1, reg2, addr;

	switch (opcode)
	{
		case MOV:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] = cpu.reg[reg2];
			break;
		case ADD:
			reg1 = cpu.memory[cpu.pc++];
			reg2 = cpu.memory[cpu.pc++];
			cpu.reg[reg1] += cpu.reg[reg2];
			break;
		case HLT:
			cpu.halted = true;
			break;
		case JMP:
			addr = cpu.memory[cpu.pc++];
			cpu.pc = addr;
			break;
		// TODO: complete instruction set
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

/*
 * Reading the assembly file and writing its instructions in
 * opcode format in memory
*/

void assemble(const char* filename)
{
	FILE* fp = fopen(filename, "r");

	if (!fp)
	{
		printf("Cannot read file '%s'\n", filename);
		exit(1);
	}

	char line[256] = {0};
	size_t mem_index = 0;

	while (fgets(line, sizeof(line), fp))
	{
		char instruction[10];
		int reg1, reg2, addr;
		if (strncmp(line, ";", 1) == 0)
		{
			// comment, ignore
			continue;
		}
		else if (sscanf(line, "%s %d %d", instruction, &reg1, &reg2) == 3)
		{
			if (strncmp(instruction, "MOV", 3) == 0)
			{
				cpu.memory[mem_index++] = MOV;
				cpu.memory[mem_index++] = reg1;
				cpu.memory[mem_index++] = reg2;
			} else if (strncmp(instruction, "ADD", 3) == 0)
			{
				cpu.memory[mem_index++] = ADD;
				cpu.memory[mem_index++] = reg1;
				cpu.memory[mem_index++] = reg2;
			}
		} else if (sscanf(line, "%s %d", instruction, &addr) == 2)
		{
			if (strncmp(instruction, "JMP", 3) == 0)
			{
				cpu.memory[mem_index++] = JMP;
				cpu.memory[mem_index++] = addr;
			}
			// TODO for jmp...
		} else if (strncmp(line, "HLT", 3) == 0)
		{
			cpu.memory[mem_index++] = HLT;
		}
	}
	fclose(fp);
}

/*
 * Running the program, incrementing the program counter for each instruction ran
*/

void cpu_run()
{
	while (!cpu.halted)
	{
		uint8_t opcode = cpu.memory[cpu.pc++];
		cpu_exec(opcode);
	}
}

/*
 * Dumping the CPU information, registers and flags
*/

void cpu_dump()
{
	printf("\n*** CPU state dump ***\nPC: 0x%x\nHalted: %d\n\n", cpu.pc, cpu.halted);
	
	for (size_t i=0; i<NUM_REGISTERS; i++)
	{
		printf("R%d: 0x%x\n", i, cpu.reg[i]);
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
		printf("%02x ", cpu.memory[i]);	
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
		printf("Usage: %s <assembly file>\n", argv[0]);
		return -1;
	}

	assemble(argv[1]);
	
	// Dumping our program
	mem_dump();
	
	reg_write(2, 0x10);
	cpu_run();
	
	// Post-mortem analysis
	cpu_dump();

	return 0;
}
