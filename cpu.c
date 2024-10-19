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

#define MEM_SIZE	256
#define NUM_REGISTERS	4

/*
 * Instruction set
 * Here, we're making a RISC (reduced instruction set computer)
 * so we're staying minimalistic.
*/

typedef enum
{
	// 0x00 -> No operation
	NOP 	= 0,

	// 0xA? -> Memory operations
	MOV 	= 0xA0,

	// 0xB? -> Arithmetic operations
	ADD 	= 0xB0,
	SUB 	= 0xB1,

	// 0xC? -> Bitwise operations
	OR	= 0xC0,
	AND	= 0xC1,
	XOR	= 0xC2,

	// 0xD? -> Input/output operations
	OUT	= 0xD0,
	IN	= 0xD1,

	// 0xE? -> Jump and comparisons
	JMP 	= 0xE0,
	JEQ 	= 0xE1,
	CMP 	= 0xE2,
	
	// 0xF? -> Misc operations
	HLT 	= 0xFF
} instruction_set_t;

/*
 * CPU structure definition
 * Contains 4 8-bit registers, memory, a program counter, a halt switch, and flags.
*/

typedef struct
{
	uint8_t reg[NUM_REGISTERS];
	uint8_t memory[MEM_SIZE];
	uint16_t pc;
	bool halted;
	bool equal_flag;
	int flag_clear_delay;
} CPU_t;

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
	uint8_t reg1, reg2, addr;

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
		char instruction[10] = {0};
		char reg1[10] = {0};
		char reg2[10] = {0};
		int addr;
		if (strncmp(line, ";", 1) == 0)
		{
			// comment, ignore
			continue;
		}
		else if (sscanf(line, "%s %[^,], %s", instruction, reg1, reg2) == 3)
		{
			//printf("SS1");
			int reg1_n = reg1[1] - '0';
			int reg2_n = reg2[1] - '0';

			if (strncmp(instruction, "MOV", 3) == 0)
			{
				cpu.memory[mem_index++] = MOV;
				cpu.memory[mem_index++] = reg1_n;
				cpu.memory[mem_index++] = reg2_n;
			} else if (strncmp(instruction, "ADD", 3) == 0)
			{
				cpu.memory[mem_index++] = ADD;
				cpu.memory[mem_index++] = reg1_n;
				cpu.memory[mem_index++] = reg2_n;
			} else if (strncmp(instruction, "CMP", 3) == 0)
			{
				cpu.memory[mem_index++] = CMP;
				cpu.memory[mem_index++] = reg1_n;
				cpu.memory[mem_index++] = reg2_n;
			} else if (strncmp(instruction, "SUB", 3) == 0)
			{
				cpu.memory[mem_index++] = SUB;
				cpu.memory[mem_index++] = reg1_n;
				cpu.memory[mem_index++] = reg2_n;
			}
			else if (strncmp(instruction, "OR", 2) == 0)
			{
			    cpu.memory[mem_index++] = OR;
			    cpu.memory[mem_index++] = reg1_n;
			    cpu.memory[mem_index++] = reg2_n;
			}
			else if (strncmp(instruction, "AND", 3) == 0)
			{
			    cpu.memory[mem_index++] = AND;
			    cpu.memory[mem_index++] = reg1_n;
			    cpu.memory[mem_index++] = reg2_n;
			}
			else if (strncmp(instruction, "XOR", 3) == 0)
			{
			    cpu.memory[mem_index++] = XOR;
			    cpu.memory[mem_index++] = reg1_n;
			    cpu.memory[mem_index++] = reg2_n;
			}
		} else if (sscanf(line, "%s %[^,], %d", instruction, reg1, &addr) == 2)
		{
			//printf("SS2");
			int reg1_n = reg1[1] - '0';

			if (strncmp(instruction, "JEQ", 3) == 0)
			{
				cpu.memory[mem_index++] = JEQ;
				cpu.memory[mem_index++] = reg1_n;
				cpu.memory[mem_index++] = addr;
			}
			else if (sscanf(line, "%s %d", instruction, &addr) == 2)
			{
				//printf("SS3");
				if (strncmp(instruction, "JMP", 3) == 0)
				{
					cpu.memory[mem_index++] = JMP;
					cpu.memory[mem_index++] = addr;
				}
			}

			else if (sscanf(line, "%s %s", instruction, reg1) == 2)
			{
			    //printf("SS4");
			    int reg1_n = reg1[1] - '0';

			    if (strncmp(instruction, "OUT", 3) == 0)
			    {
				cpu.memory[mem_index++] = OUT;
				cpu.memory[mem_index++] = reg1_n;
			    }

			    if (strncmp(instruction, "IN", 2) == 0)
			    {
				cpu.memory[mem_index++] = IN;
				cpu.memory[mem_index++] = reg1_n;
			    }
			}	
		} 
	
		else if (strncmp(line, "HLT", 3) == 0)
		{
			cpu.memory[mem_index++] = HLT;
		}
		else if (strncmp(line, "NOP", 3) == 0)
		{
			cpu.memory[mem_index++] = NOP;
		}
	}
	fclose(fp);
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

		switch (cpu.memory[i])
		{
			// Instructions (colored background)
			case 0xa0:
				printf("\e[42m%02x\e[0m ", cpu.memory[i]);
				break;
		
			case 0xb0:
			case 0xb1:
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
		printf("Usage: %s <assembly file>\n", argv[0]);
		return -1;
	}

	assemble(argv[1]);
	
	// Dumping our program
	mem_dump();

	reg_write(1, 0x68);
	reg_write(2, 0x69);
	reg_write(3, 0x21);
	cpu_run();
	
	// Post-mortem analysis
	cpu_dump();

	return 0;
}
