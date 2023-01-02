#include <stdlib.h>
#include <stdint.h>
#include "../headers/decode.h"
#include "../headers/utils.h"

int8_t W_dest;
int32_t W_res;
int32_t W_valM;
int W_sig_MR;
int W_sig_WR;

void write_back()
{
	regfile.sig_w = W_sig_WR;
	regfile.w_in = W_dest;
	regfile.w_val = multiplex(W_res, W_valM, W_sig_MR);
	access_reg_file(&regfile);
}
