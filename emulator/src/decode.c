#include <stdlib.h>
#include <stdint.h>
#include "../headers/exec.h"
#include "../headers/decode.h"
#include "../headers/utils.h"

int32_t D_next_pc = 3;
int8_t D_opcode;
int8_t D_dest;
int8_t D_op1;
int8_t D_op2;
int32_t D_imm;
int D_sig_I;
int D_sig_J;
int D_sig_B;
int8_t D_func;
int D_sig_MR;
int D_sig_MW;
int D_sig_WR;
int D_cur_pc;
int D_sig_hlt;

static int32_t tmp_E_next_pc;
static int8_t tmp_E_opcode;
static int8_t tmp_E_dest;
static int32_t tmp_E_valA;
static int32_t tmp_E_valB;
static int32_t tmp_E_valD;
static int32_t tmp_E_imm;
static int tmp_E_sig_I; 
static int tmp_E_sig_J; 
static int tmp_E_sig_B; 
static int8_t tmp_E_func;
static int tmp_E_sig_MR; 
static int tmp_E_sig_MW; 
static int tmp_E_sig_WR; 
static int tmp_E_cur_pc;
static int tmp_E_sig_hlt;

struct regfile_struct regfile;

static int32_t sign_extend(int32_t imm)
{
	if (imm >> 16) imm = imm | 0xfffe0000;
	else imm = imm & 0x0001ffff;
	return imm;
}

void access_reg_file(struct regfile_struct* rg_file)
{
	if (!rg_file->sig_w) {
		rg_file->r_out1 = rg_file->reg_array[rg_file->r_in1];
		rg_file->r_out2 = rg_file->reg_array[rg_file->r_in2];
		rg_file->r_out3 = rg_file->reg_array[rg_file->r_in3];
	}
	else {
		rg_file->reg_array[rg_file->w_in] = rg_file->w_val;
	}
}
void decode_update_pipeline_regs()
{
	E_next_pc = tmp_E_next_pc;
	E_opcode = tmp_E_opcode;
	E_dest = tmp_E_dest;
	E_valA = tmp_E_valA;
	E_valB = tmp_E_valB;
	E_valD = tmp_E_valD;
	E_imm = tmp_E_imm;
	E_sig_I = tmp_E_sig_I;
	E_sig_J = tmp_E_sig_J;
	E_sig_B = tmp_E_sig_B;
	E_func = tmp_E_func;
	E_sig_MR = tmp_E_sig_MR;
	E_sig_MW = tmp_E_sig_MW;
	E_sig_WR = tmp_E_sig_WR;
	E_cur_pc = tmp_E_cur_pc;
	E_sig_hlt = tmp_E_sig_hlt;
}

void decode()
{
	regfile.r_in1 = D_op1;
	regfile.r_in2 = D_op2;
	regfile.r_in3 = D_dest;
	access_reg_file(&regfile);
	tmp_E_next_pc = D_next_pc;
	tmp_E_opcode = D_opcode;
	tmp_E_dest = D_dest;
	tmp_E_valA = regfile.r_out1;
	tmp_E_valB = regfile.r_out2;
	tmp_E_valD = regfile.r_out3;
	tmp_E_imm = sign_extend(D_imm);
	tmp_E_sig_I = D_sig_I;
	tmp_E_sig_J = D_sig_J;
	tmp_E_sig_B = D_sig_B;
	tmp_E_func = D_func;
	tmp_E_sig_MR = D_sig_MR;
	tmp_E_sig_MW = D_sig_MW;
	tmp_E_sig_WR = D_sig_WR;
	tmp_E_cur_pc = D_cur_pc;
	tmp_E_sig_hlt = D_sig_hlt;
}
