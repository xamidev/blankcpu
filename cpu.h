#ifndef CPU_H
#define CPU_H

#define MEM_SIZE	256
#define NUM_REGISTERS	8
#define BUF_MAX     256
#define INF_LOOP_THRESHOLD 10000

/*
 * Instruction set
 * Here, we're making a RISC (reduced instruction set computer)
 * so we're staying minimalistic.
*/

typedef enum
{
	// 0xA? -> Memory operations
	MOV 	= 0xA0,
	PUT	= 0xA1,

	// 0xB? -> Arithmetic operations
	ADD 	= 0xB0,
	SUB 	= 0xB1,
	MUL	= 0xB2,
	DIV	= 0xB3,

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
	NOP	= 0xFE,
	HLT 	= 0xFF
} instruction_set_t;

/*
 * CPU structure definition
 * Contains 8-bit registers, memory, a program counter, a halt switch, and flags.
*/

typedef struct
{
	uint8_t reg[NUM_REGISTERS];
	uint8_t memory[MEM_SIZE];
	uint16_t pc;
	bool halted;
	bool equal_flag;
	int flag_clear_delay;
	int increment;
} CPU_t;

#endif
