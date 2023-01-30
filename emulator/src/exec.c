#include <stdlib.h>
#include <stdint.h>
#include "../headers/defs.h"
#include "../headers/utils.h"
#include "../headers/mem.h"

int32_t E_next_pc = 2;
int8_t E_opcode;
int8_t E_dest;
int32_t E_valA;
int32_t E_valB;
int32_t E_valD;
int32_t E_imm;
int E_sig_I;
int E_sig_J;
int E_sig_B;
int8_t E_func;
int E_sig_MR;
int E_sig_MW;
int E_sig_WR;

static int32_t tmp_M_next_pc;
static int8_t tmp_M_dest;
static int32_t tmp_M_res;
static int32_t tmp_M_valA;
static int32_t tmp_M_valD;
static int tmp_M_sig_J;
static int tmp_M_sig_B;
static int tmp_M_sig_MW;
static int tmp_M_sig_MR;
static int tmp_M_sig_WR;

struct alu_struct {
	int32_t op1_in;
	int32_t op2_in;
	int32_t out;
	int8_t opcode_in;
	int8_t func_in;
};

struct alu_struct alu;

static void exec_alu(struct alu_struct* alu_strct)
{
	switch (alu_strct->opcode_in)
	{
		case MOVE_OPCODE: alu_strct->out = alu_strct->op1_in;
				  break;
		case LOADI_OPCODE: alu_strct->out = alu_strct->op2_in;
				   break;
		case BIN_ALU_OP_OPCODE: switch (alu_strct->func_in)
					{
						case 0: alu_strct->out = alu_strct->op1_in + alu_strct->op2_in;
							break;
						case 1: alu_strct->out = alu_strct->op1_in >> 1;
							break;
						case 2: alu_strct->out = alu_strct->op1_in << 1;
							break;
						case 3: alu_strct->out = alu_strct->op1_in & alu_strct->op2_in;
							break;
						case 4: alu_strct->out = alu_strct->op1_in | alu_strct->op2_in;
							break;
						case 5: alu_strct->out = ~alu_strct->op1_in;
							break;
						case 7: alu_strct->out = alu_strct->op1_in >> 8;
							break;
						case 8: alu_strct->out = alu_strct->op1_in << 8;
							break;
					}
					break;
		case ADDI_OPCODE: alu_strct->out = alu_strct->op1_in + alu_strct->op2_in;
				  break;
		case BRANCH_OPCODE: alu_strct->out = alu_strct->op1_in + alu_strct->op2_in;
				    break;	
	}
}

static int branch_taken(int32_t cond, int8_t func, int b_sig)
{
	if (b_sig)
	{
		switch (func)
		{
			case BRE_FUNC: return cond == 0;
			case BRL_FUNC: return cond < 0;
			case BRG_FUNC: return cond > 0;
		}
	}
	return 0;
}

void execute_update_pipeline_regs()
{
	M_next_pc = tmp_M_next_pc;
	M_dest = tmp_M_dest;
	M_res = tmp_M_res;
	M_valA = tmp_M_valA;
	M_valD = tmp_M_valD;
	M_sig_J = tmp_M_sig_J;
	M_sig_B = tmp_M_sig_B;
	M_sig_MW = tmp_M_sig_MW;
	M_sig_MR = tmp_M_sig_MR;
	M_sig_WR = tmp_M_sig_WR;
}

void execute()
{
	alu.op1_in = multiplex(E_valA, E_next_pc, E_sig_B);
	alu.op2_in = multiplex(E_valB, E_imm, E_sig_I);
	alu.opcode_in = E_opcode;
	alu.func_in = E_func;
	exec_alu(&alu);

	tmp_M_next_pc = E_next_pc;
	tmp_M_dest = E_dest;
	tmp_M_res = alu.out;
	tmp_M_valA = E_valA;
	tmp_M_valD = E_valD;
	tmp_M_sig_J = E_sig_J;
	tmp_M_sig_B = branch_taken(E_valD, E_func, E_sig_B);
	tmp_M_sig_MW = E_sig_MW;
	tmp_M_sig_MR = E_sig_MR;
	tmp_M_sig_WR = E_sig_WR;
}
