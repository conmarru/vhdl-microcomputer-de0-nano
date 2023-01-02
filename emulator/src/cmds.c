#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "../headers/fetch.h"
#include "../headers/decode.h"
#include "../headers/exec.h"
#include "../headers/mem.h"
#include "../headers/wrback.h"
#include "../headers/disasm.h"

const char* CMDS[] = { "next", "n", "print", "p", "quit", "q" };

const char* REG_STRINGS[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12",
			      "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24",
		              "r25", "r26", "r27", "r28", "r29", "r30", "r31" };


char** parse_uinput(char* input, int* argc)
{
	if (!input) return NULL;
	char** args = (char**)malloc(sizeof(char*)*1);	
	args[0] = NULL;

	unsigned int i = 0;	
	unsigned int j = 0;
	unsigned int counter = 0;
	char* arg = (char*)calloc(1, sizeof(char));
	arg[0] = 0x00;

	while (input[i] != 0x00)
	{
		if (input[i] == ' ')
		{
			arg = realloc(arg, ++j);
			arg[j-1] = 0x00;
			args[counter] = arg;
			args = realloc(args, ++counter);	
			args[counter] = NULL;
			arg = (char*)calloc(1, sizeof(char));
			j = 0;
			while (input[i] == ' ')
			{	
				i++;
			}
		}
		else
		{
			arg = realloc(arg, ++j);
			arg[j-1] = input[i++];	
		}	
	}

	arg = realloc(arg, ++j);
	arg[j-1] = 0x00;
	args[counter++] = arg;

	*argc = counter;
	return args;
}

void reg_dump(int32_t* regfile, const char** reg_idnts, int32_t pc_val)
{
	printf("==================== Registers ====================\n");
	printf("PC = 0x%.8x\n", pc_val);
	for (int i = 0; i < 16; i++)	
	{
		char* fstr = (i < 10) ? "%s = %-10d\t\t\t%s = %d\n" : "%s = %-9d\t\t\t%s = %d\n";
		printf(fstr , reg_idnts[i], regfile[i], reg_idnts[i+16], regfile[i+16]);
	}
}

void print_mem(int32_t* mem, uint32_t start_addr, uint32_t end_addr, bool disasm_flag, bool verbose_flag, bool mflag, uint32_t maddr)
{
	if (verbose_flag) printf("===================== Memory ======================\n");
	for (uint32_t i = start_addr; i < end_addr; i++)
	{
		char* fstr;
		if (disasm_flag)
		{
			if (mflag) fstr = (i == maddr) ? "0x%.8x: %.8x (*)" : "0x%.8x: %.8x";
			else fstr = "0x%.8x: %.8x";
		}
		else
		{
			if (mflag) fstr = (i == maddr) ? "0x%.8x: %.8x (*)\n" : "0x%.8x: %.8x\n";
			else fstr = "0x%.8x: %.8x\n";
		}
		printf(fstr, i, mem[i]);
		if (disasm_flag)
		{
			if (mflag && i == maddr) printf(" [ ");
			else printf("%5s[ ", "");
			uint32_t instr = ((int32_t)mem[i]);
			disasm_instr(instr);
			printf(" ]\n");
		}

	}
}

void load_program(int32_t* mem, uint8_t* bin_data, int size, unsigned int memsize)
{
	unsigned int j = 0;
	for (int i = 0; i < size; i+=4)
	{
		if (j >= memsize) return;
		int32_t instr = 0;
		instr = ((int32_t)bin_data[i]) << 16 | ((int32_t)bin_data[i+1]) << 24 | ((int32_t)bin_data[i+2]) | ((int32_t)bin_data[i+3]) << 8;
		mem[j++] = instr;
	}
}

void free_userargs(char** args, int argc)
{
	for (int i = 0; i < argc; i++)
	{
		free(args[i++]);		
	}
	free(args);
}
