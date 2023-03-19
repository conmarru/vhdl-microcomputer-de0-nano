#include <stdint.h>

extern int8_t W_dest;
extern int32_t W_res;
extern int32_t W_valM;
extern int W_sig_MR;
extern int W_sig_WR;
extern int W_sig_hlt;
extern int wr_hlt_flag;

void write_back(void);
