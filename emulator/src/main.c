#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include "../headers/defs.h"
#include "../headers/disasm.h"
#include "../headers/cmds.h"
#include "../headers/utils.h"

#define REG_NUM 32
#define MEM_32 4294967296
#define MAX_CMD_LEN 50

#define MAX_FILENAME 200

const uint32_t EXTRACT_MASK = 0x0000001f;
static uint8_t flags = 0;

static uint8_t* mem;
static int32_t reg[REG_NUM] = { 0 };

const char* REG_STRINGS[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12",
			      "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24",
		              "r25", "r26", "r27", "r28", "r29", "r30", "r31" };

int err_val = 0;

static const char* CMDS[] = { "next", "n", "print", "p", "quit", "q" };

static char user_input[MAX_CMD_LEN+1] = { 0x00 };

static bool jmp_flag = false;

static uint32_t pc = 0;

static void load_program(uint8_t* bin_data, int size)
{
	for (int i = 0; i < size; i+=2)
	{
		mem[i+1] = bin_data[i];
		mem[i] = bin_data[i+1];
	}
}

static void reg_dump()
{
	printf("==================== Registers ====================\n");
	printf("PC = 0x%.8x\n", pc);
	for (int i = 0; i < 16; i++)	
	{
		char* fstr = (i < 10) ? "%s = %-10d\t\t\t%s = %d\n" : "%s = %-9d\t\t\t%s = %d\n";
		printf(fstr , REG_STRINGS[i], reg[i], REG_STRINGS[i+16], reg[i+16]);
	}
}

static void print_mem(uint32_t start_addr, uint32_t end_addr, bool disasm_flag, bool verbose_flag)
{
	if (verbose_flag) printf("===================== Memory ======================\n");
	for (uint32_t i = start_addr; i < end_addr; i+=4)
	{
		char* fstr = (disasm_flag) ? "0x%.8x: %.2x %.2x %.2x %.2x" : "0x%.8x: %.2x %.2x %.2x %.2x\n";
		printf(fstr, i, mem[i], mem[i+1], mem[i+2], mem[i+3]);
		if (disasm_flag)
		{
			printf("\t[ ");
			uint32_t b1 = ((int32_t)mem[i]) << 24;
			uint32_t b2 = ((int32_t)mem[i+1]) << 16;
			uint32_t b3 = ((int32_t)mem[i+2]) << 8;
			uint32_t b4 = (int32_t)mem[i+3];
			uint32_t instr = b1 | b2 | b3 | b4;
			disasm_instr(instr);
			printf(" ]\n");
		}

	}
}

static void exec(int flag)
{	
	uint32_t instr;
	uint8_t dest_reg;
	uint8_t op1_reg;
	uint8_t op2_reg;
	uint8_t cond_reg;
	uint8_t src_reg;
	uint8_t func;
	int32_t immediate;
	uint32_t offset;
	while (1)
	{
		if (flag & 0x04)
		{
			bool break_cond = false;
			while (!break_cond)
			{
				printf("> ");
				char* r = fgets(user_input, MAX_CMD_LEN, stdin);	
				user_input[strlen(user_input)-1] = (user_input[strlen(user_input)-1] == '\n') ? 0x00 : user_input[strlen(user_input)-1];
				int argc = 0;
				char** user_args = parse_uinput(user_input, &argc);
				if (!r) exit(1);

				if (!strcmp(user_args[0], CMDS[0]) || !strcmp(user_args[0], CMDS[1]))
				{
					break_cond = true;
				}
				else if (!strcmp(user_args[0], CMDS[2]) || !strcmp(user_args[0], CMDS[3]))
				{
					bool disasm_flag = true;
					switch (argc)
					{
						case 1 : reg_dump();
							 print_mem(0, pc+4, disasm_flag, true);
							 break;
						case 3 : if (!strcmp(user_args[1], "-x"))
							 {
							 	uint32_t maddr = strtohex(user_args[2]);				 
								if (err_val == 1)
								{
									printf("ERROR: invalid address\n");
								}
								else
								{
									print_mem(maddr - (maddr % 4), (maddr - (maddr % 4)) + 4, false, false);
								}
							 }
					}
				}
				else if (!strcmp(user_args[0], CMDS[4]) || !strcmp(user_args[0], CMDS[5]))
				{

					free_userargs(user_args, argc);	
					exit(0);
				}
				else
				{
					printf("ERROR: Command not found\n");
				}
				free_userargs(user_args, argc);	
			}
		}
		instr = (mem[pc] << 24) | (mem[pc+1] << 16) | (mem[pc+2] << 8) | mem[pc+3];

		switch (instr >> OPCODE_SHIFT_VAL)
		{
			case 0: break;
			case 1: return;
			case 2:	dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				op1_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
				reg[dest_reg] = reg[op1_reg];
				break;
			case 3: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				immediate = instr & 0x003fffff;
				if (immediate >> 21) immediate |= 0xffc00000;
				reg[dest_reg] = immediate;
				break;
			case 4: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				op1_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
				op2_reg = (instr >> OP2_REG_SHIFT_VAL) & EXTRACT_MASK;
				func = instr & EXTRACT_MASK;
				switch (func)
				{
					case 0: reg[dest_reg] = reg[op1_reg] + reg[op2_reg];
						break;
					case 1: reg[dest_reg] = reg[op1_reg] >> reg[op2_reg];
						break;
					case 2: reg[dest_reg] = reg[op1_reg] << reg[op2_reg];
						break;
					case 3: reg[dest_reg] = reg[op1_reg] & reg[op2_reg];
						break;
					case 4: reg[dest_reg] = reg[op1_reg] | reg[op2_reg];
						break;
				}
				break;
			case 5: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				op1_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
				immediate = instr & 0x00007fff;
				if (immediate >> 16) immediate |= 0xffff8000;
				reg[dest_reg] = reg[op1_reg] + immediate;
				break;
			case 6: pc = reg[29];
				jmp_flag = true;
				break;
			case 7: cond_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				func = (instr >> 19) & 0x00000007;
				offset = instr & 0x0007ffff;
				offset = (offset >> 18) ? offset | 0xfff80000 : offset;
				int32_t extracted_offset = (int32_t)offset;
				switch (func)
				{
					case 0 : jmp_flag = (reg[cond_reg] == 0);
						 pc = (jmp_flag) ? pc + extracted_offset : pc;
						 break;
					case 1 : jmp_flag = (reg[cond_reg] < 0);
						 pc = (jmp_flag) ? pc + extracted_offset : pc;
						 break;
					case 2 : jmp_flag = (reg[cond_reg] > 0);
						 pc = (jmp_flag) ? pc + extracted_offset : pc;
						 break;
				}	
				break;
			case 8: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				int32_t b1 = ((int32_t)mem[reg[29]]) << 24;
				int32_t b2 = ((int32_t)mem[reg[29]+1]) << 16;
				int32_t b3 = ((int32_t)mem[reg[29]+2]) << 8;
				int32_t b4 = (int32_t)mem[reg[29]+3];
				mem[dest_reg] = b1 | b2 | b3 | b4;
				break;
			case 9: src_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				mem[reg[29]] = (uint8_t) ((reg[src_reg] & 0xff000000) >> 24);
				mem[reg[29]+1] = (uint8_t) ((reg[src_reg] & 0x00ff0000) >> 16);
				mem[reg[29]+2] = (uint8_t) ((reg[src_reg] & 0x0000ff00) >> 8);
				mem[reg[29]+3] = (uint8_t) reg[src_reg] & 0x000000ff;
				break;
			case 10: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				 op1_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				 reg[dest_reg] = ~reg[op1_reg];
				 break;
		}
		pc = (jmp_flag) ? pc : pc + 4;
		jmp_flag = false;	
	}
}

static void parse_cli_args(int argc, char** args, char* filename)
{
	int c = 0; 
	opterr = 0;
	while (1)
	{
		static struct option long_options[] = {
			{"input", required_argument, NULL, 'i'},
			{"regdump", no_argument, NULL, 'r'},
			{"memdump", no_argument, NULL, 'm'},
			{"interactive", no_argument, NULL, 'I'},
			{NULL, 0, NULL, 0}
		};

		int option_index = 0;
		c = getopt_long(argc, args, "i:rmI", long_options, &option_index);

		if (c == -1)
		{
			return;
		}

		switch (c)
		{
			case 'i' : memcpy(filename, optarg, MAX_FILENAME);
				   break;
			case 'r' : flags |= 1;
				   break;
			case 'm' : flags |= 2;
				   break;
			case 'I' : flags |= 4;
				   break;
			case '?' : printf("usage:./main -i <filename> [flags]\n");
				   printf("flags:\t-r, --regdump\n");
				   printf("\t-m, --memdump\n");
				   printf("\t-I, --interactive\n");
				   exit(1);
		}
	}
}

int main(int argc, char** args)
{
	mem = (uint8_t*)calloc(MEM_32, sizeof(uint8_t));
	char* filename = (char*)calloc(MAX_FILENAME+1, sizeof(char));
	parse_cli_args(argc, args, filename);
	FILE* input_file = fopen(filename, "rb");
	free(filename);
	if (!input_file)
	{
		printf("ERROR: Failed to open input file\n");
		exit(1);
	}
	int ifd = fileno(input_file);
	int fsize = lseek(ifd, 0, SEEK_END);

	void* buf = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, ifd, 0);
	if (!buf) exit(1);

	uint8_t* binary_data = (uint8_t*)calloc(fsize,sizeof(uint8_t));
	memcpy(binary_data, buf, fsize);
	load_program(binary_data, fsize);
	exec(flags);
	if (!(flags & 0x04))
	{
		if (flags & 0x01)
		{
			reg_dump();
		}
		if (flags & 0x02)
		{
			print_mem(0, 0x00000100, false, true);
		}
	}
	munmap(buf, fsize);
	return 0;
}
