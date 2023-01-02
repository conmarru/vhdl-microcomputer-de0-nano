#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/cmds.h"
#include "../headers/utils.h"
#include "../headers/defs.h"

static int32_t* mem;
static int32_t reg[REG_NUM] = { 0 };

static char user_input[MAX_CMD_LEN+1] = { 0x00 };

static bool jmp_flag = false;

static uint32_t pc = 0;

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
						case 1 : reg_dump(reg, REG_STRINGS, pc);
							 int32_t pc_start = 0;
							 if ((int32_t)pc-2 < 0) pc_start = 0;
							 else pc_start = pc-2;
							 print_mem(mem, pc_start, pc+3, disasm_flag, true, true, pc);
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
									print_mem(mem, maddr, maddr + 1, false, false, false, 0);
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
		instr = mem[pc];
		switch (instr >> OPCODE_SHIFT_VAL)
		{
			case 0: break;
			case 1: return;
			case 2:	dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				op1_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
				reg[dest_reg] = reg[op1_reg];
				break;
			case 3: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				immediate = instr & 0x0001ffff; 
				if (immediate >> 16) immediate |= 0xfffe0000;
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
					case 1: reg[dest_reg] = reg[op1_reg] & reg[op2_reg];
						break;
					case 2: reg[dest_reg] = reg[op1_reg] | reg[op2_reg];
						break;
				}
				break;
			case 5: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				op1_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
				immediate = instr & 0x0001ffff; 
				if (immediate >> 16) immediate |= 0xfffe0000;
				reg[dest_reg] = reg[op1_reg] + immediate;
				break;
			case 6: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				pc = reg[dest_reg];
				jmp_flag = true;
				break;
			case 7: cond_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				func = instr & EXTRACT_MASK;
				offset = (instr & 0x003fffe0) >> 5;
				offset = (offset >> 16) ? offset | 0xfffe0000 : offset;
				int32_t extracted_offset = (int32_t)offset;
				switch (func)
				{
					case 0 : pc = (reg[cond_reg] == 0) ? pc + extracted_offset : pc;
						 break;
					case 1 : pc = (reg[cond_reg] < 0) ? pc + extracted_offset : pc;
						 break;
					case 2 : pc = (reg[cond_reg] > 0) ? pc + extracted_offset : pc;
						 break;
				}	
				break;
			case 8: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				op1_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
				reg[dest_reg] = ((uint32_t)mem[reg[op1_reg]]);
				break;
			case 9: src_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
				dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				mem[reg[dest_reg]] = reg[src_reg];
				break;
			case 10: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				 op1_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				 reg[dest_reg] = ~reg[op1_reg];
				 break;
			case 11: dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				 op1_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
				 func = instr & EXTRACT_MASK;
				 switch (func)
				 {
					 case 0: reg[dest_reg] = reg[op1_reg] >> 1;
						 break;
					 case 1: reg[dest_reg] = reg[op1_reg] << 1;
						 break;
					 case 2: reg[dest_reg] = reg[op1_reg] >> 8;
						 break;
					 case 3: reg[dest_reg] = reg[op1_reg] << 8;
						 break;
				 }
		}
		if (!jmp_flag) pc = pc + 1;
		jmp_flag = false;	
	}
}

void seq_run(uint8_t* binary_data, int fsize, unsigned int mem_size)
{	
	mem = (int32_t*)calloc(mem_size, sizeof(uint8_t));
	load_program(mem, binary_data, fsize, mem_size);
	exec(flags);
	if (!(flags & 0x04))
	{
		if (flags & 0x01)
		{
			reg_dump(reg, REG_STRINGS, pc);
		}
		if (flags & 0x02)
		{
			print_mem(mem, 0, 0x00000404, false, true, false, 0);
		}
	}
}
