#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../headers/decode.h"
#include "../headers/utils.h"

bool wr_hlt_flag = false;

int8_t W_dest;
int32_t W_res;
int32_t W_valM;
int W_sig_MR;
int W_sig_WR;
int W_sig_hlt;

void write_back(void)
{
	wr_hlt_flag = W_sig_hlt;
	regfile.sig_w = W_sig_WR;
	regfile.w_in = W_dest;
	regfile.w_val = multiplex(W_res, W_valM, W_sig_MR);
	access_reg_file(&regfile);
	regfile.sig_w = 0;
}
