#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../headers/cmds.h"
#include "../headers/fetch.h"
#include "../headers/decode.h"
#include "../headers/exec.h"
#include "../headers/mem.h"
#include "../headers/wrback.h"
#include "../headers/utils.h"

static char user_input[MAX_CMD_LEN+1] = { 0x00 };

static void init_pipeline(unsigned int mem_size, uint8_t* binary_data, int fsize)
{
	alloc_instr_mem(mem_size);
	alloc_data_mem(mem_size);	
	load_program(instr_mem.mem_array, binary_data, fsize, mem_size);
	pc_str.next_pc_out = 4;
}

static void free_mem()
{
	free_instr_mem();
	free_data_mem();
}

static void exec_pipelined(uint8_t flags)
{	
	int next_count = 0;
	while (1)
	{
		if (flags & 0x04)
		{
			bool break_cond = false;	
			while (!break_cond && !next_count)
			{
				printf("> ");
				char* r = fgets(user_input, MAX_CMD_LEN, stdin);	
				user_input[strlen(user_input)-1] = (user_input[strlen(user_input)-1] == '\n') ? 0x00 : user_input[strlen(user_input)-1];
				int argc = 0;
				char** user_args = parse_uinput(user_input, &argc);
				if (!r)
				{
					printf("ERROR: Couldn't parse user input\n");
					exit(1);
				}
				if (!strcmp(user_args[0], CMDS[0]) || !strcmp(user_args[0], CMDS[1]))
				{
					break_cond = true;	
					int skip = 1;
					if (argc == 2)
					{
						skip = strtouint(user_args[1]);	
					}
					next_count = skip;
				}
				else if (!strcmp(user_args[0], CMDS[2]) || !strcmp(user_args[0], CMDS[3]))
				{
					if (argc == 1 || !strcmp(user_args[1], "-a"))
					{
						printf("fetch stage:");
						printf("\t\tdecode stage:");
						printf("\t\texecute stage:");
						printf("\t\tmemory stage:");
						printf("\t\twrite-back stage:\n");
						printf("PC: %-10d%10sD_next_pc: %-10d%3sE_next_pc: %-10d%3sM_next_pc: %-10d%3sW_dest: %d\n", pc_str.pc_out, "", D_next_pc, "", E_next_pc, "", M_next_pc, "", W_dest);
						printf("%24sD_opcode: %-2d%12sE_opcode: %-2d%12sM_dest: %-2d%14sW_res: %d\n", "", D_opcode, "", E_opcode, "", M_dest, "", W_res);
						printf("%24sD_dest: %-2d%14sE_dest: %-2d%14sM_res: %-10d%7sW_valM: %d\n", "", D_dest, "", E_dest, "", M_res, "", W_valM);
						printf("%24sD_op1: %-10d%7sE_valA: %-10d%6sM_valA: %-10d%6sW_sig_MR: %d\n", "", D_op1, "", E_valA, "", M_valA, "", W_sig_MR);
						printf("%24sD_op2: %-10d%7sE_valB: %-10d%6sM_valD: %-10d%6sW_sig_WR: %d\n", "", D_op2, "", E_valB, "", M_valD, "", W_sig_WR);
						printf("%24sD_imm: %-10d%7sE_imm: %-10d%7sM_sig_J: %d\n", "", D_imm, "", E_imm, "", M_sig_J);
						printf("%24sD_sig_I: %d%14sE_sig_I: %d%14sM_sig_B: %d\n", "", D_sig_I, "", E_sig_I, "", M_sig_B);
						printf("%24sD_sig_J: %d%14sE_sig_J: %d%14sM_sig_MW: %d\n", "", D_sig_J, "", E_sig_J, "", M_sig_MW);
						printf("%24sD_sig_B: %d%14sE_sig_B: %d%14sM_sig_MR: %d\n", "", D_sig_B, "", E_sig_B, "", M_sig_MR);
						printf("%24sD_func: %d%15sE_func: %d%15sM_sig_WR: %d\n", "", D_func, "", E_func, "", M_sig_WR);
						printf("%24sD_sig_MR: %d%13sE_sig_MR: %d\n", "", D_sig_MR, "", E_sig_MR);
						printf("%24sD_sig_MW: %d%13sE_sig_MW: %d\n", "", D_sig_MW, "", E_sig_MW);
						printf("%24sD_sig_WR: %d%13sE_sig_WR: %d\n", "", D_sig_WR, "", E_sig_WR);
						printf("%24s          %14sE_valD: %d\n", "", "", E_valD);
						printf("\n");
						int32_t pc_start = 0;
						if (pc_str.pc_out-2 < 0) pc_start = 0;
						else pc_start = pc_str.pc_out-2;
						print_mem(instr_mem.mem_array, pc_start, pc_str.pc_out+3, true, true, true, pc_str.pc_out, true);
					}
					else if (!strcmp(user_args[1], "-r"))
					{
						reg_dump(regfile.reg_array, REG_STRINGS, pc_str.pc_out);
					}
					else if (argc == 3 && !strcmp(user_args[1], "-x"))
					{
						uint32_t maddr = strtohex(user_args[2]);				 
						if (err_val == 1)
						{
							printf("ERROR: invalid address\n");
						}
						else
						{
							print_mem(data_mem.mem_array, maddr, maddr + 1, false, false, false, 0, true);
						}
						err_val = 0;
					}
				}
				else if (!strcmp(user_args[0], CMDS[4]) || !strcmp(user_args[0], CMDS[5]))
				{
					//free_userargs(user_args, argc);
					exit(0);
				}
				else
				{
					printf("ERROR: Unknown command\n");
				}
				//free_userargs(user_args, argc);
			}
			next_count--;
		}
		fetch();
		decode();
		execute();
		memory();
		write_back();
		fetch_update_pipeline_regs();
		decode_update_pipeline_regs();
		execute_update_pipeline_regs();
		memory_update_pipeline_regs();
		if (hlt_flag) return;
	}
}

// if hlt instr fetched ask to terminate execution (but finish pipeline)
void run(unsigned int mem_size, uint8_t* binary_data, int fsize)
{
	init_pipeline(mem_size, binary_data, fsize);

	exec_pipelined(flags);

	if (!(flags & 0x04))
	{
		if (flags & 0x01)
		{
			reg_dump(regfile.reg_array, REG_STRINGS, pc_str.pc_out);
		}
	}

	free_mem();

}
