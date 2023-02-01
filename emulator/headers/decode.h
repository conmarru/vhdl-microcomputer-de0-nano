#include <stdint.h>
#include "defs.h"

extern int32_t D_next_pc;
extern int8_t D_opcode;
extern int8_t D_dest;
extern int8_t D_op1;
extern int8_t D_op2;
extern int32_t D_imm;
extern int D_sig_I;
extern int D_sig_J;
extern int D_sig_B;
extern int8_t D_func;
extern int D_sig_MR;
extern int D_sig_MW;
extern int D_sig_WR;
extern int D_cur_pc;
extern int D_sig_hlt;

struct regfile_struct {
	int8_t r_in1;
	int8_t r_in2;
	int8_t r_in3;
	int8_t w_in;
	int32_t w_val;
	int sig_w;
	int32_t r_out1;
	int32_t r_out2;
	int32_t r_out3;
	int32_t reg_array[REG_NUM];
};

extern struct regfile_struct regfile;

void access_reg_file(struct regfile_struct* rg_file);
void decode_update_pipeline_regs();
void decode();
