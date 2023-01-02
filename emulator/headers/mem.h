#include <stdint.h>

extern int32_t M_next_pc;
extern int8_t M_dest;
extern int32_t M_res;
extern int32_t M_valA;
extern int32_t M_valD;
extern int M_sig_J;
extern int M_sig_B;
extern int M_sig_MW;
extern int M_sig_MR;
extern int M_sig_WR;

void memory_update_pipeline_regs();
void alloc_data_mem(unsigned int size);
void free_data_mem();
void memory();
