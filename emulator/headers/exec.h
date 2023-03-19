#include <stdint.h>

extern int32_t E_next_pc;
extern int8_t E_opcode;
extern int8_t E_dest;
extern int32_t E_valA;
extern int32_t E_valB;
extern int32_t E_valD;
extern int32_t E_imm;
extern int E_sig_I;
extern int E_sig_J;
extern int E_sig_B;
extern int8_t E_func;
extern int E_sig_MR;
extern int E_sig_MW;
extern int E_sig_WR;
extern int E_cur_pc;
extern int E_sig_hlt;

void execute_update_pipeline_regs(void);
void execute(void);
