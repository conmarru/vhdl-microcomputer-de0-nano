#include <stdlib.h>
#include <stdint.h>
#include "../headers/defs.h"
#include "../headers/utils.h"
#include "../headers/fetch.h"
#include "../headers/wrback.h"

int32_t M_next_pc = 1;
int8_t M_dest;
int32_t M_res;
int32_t M_valA; // addr if mem operation
int32_t M_valD; 
int M_sig_J;
int M_sig_B;
int M_sig_MW;
int M_sig_MR;
int M_sig_WR;

static int32_t tmp_F_next_pc;
static int8_t tmp_W_dest;
static int32_t tmp_W_res;
static int tmp_W_valM;
static int tmp_W_sig_MR;
static int tmp_W_sig_WR;

struct data_mem_struct {
	int32_t addr_in;
	int read_in;
	int write_in;
	int32_t data_in;
	int32_t out;
	int32_t* mem_array;
};

static struct data_mem_struct data_mem;

void alloc_data_mem(unsigned int size)
{
	data_mem.mem_array = (int32_t*)malloc(sizeof(int32_t)*size);
}

void access_data_mem(struct data_mem_struct* mem)
{
	if (mem->read_in)
	{
		mem->out = mem->mem_array[(uint32_t)mem->addr_in];	
	}
	else if (mem->write_in)
	{
		mem->mem_array[(uint32_t)mem->addr_in] = mem->data_in;
	}
}

void free_data_mem()
{
	if (data_mem.mem_array) free(data_mem.mem_array);
}

void memory_update_pipeline_regs()
{
	update_pc(tmp_F_next_pc);
	W_dest = tmp_W_dest;
	W_res = tmp_W_res;
	W_valM = tmp_W_valM;
	W_sig_MR = tmp_W_sig_MR;	
	W_sig_WR = tmp_W_sig_WR;
}

void memory()
{
	data_mem.addr_in = M_valA;
	data_mem.read_in = M_sig_MR;
	data_mem.write_in = M_sig_MW;
	data_mem.data_in = M_valD;
	access_data_mem(&data_mem);
	tmp_F_next_pc = multiplex(multiplex(M_next_pc, M_res, M_sig_B), M_valD, M_sig_J);
	tmp_W_res = M_res;
	tmp_W_dest = M_dest;
	tmp_W_valM = data_mem.out;
	tmp_W_sig_MR = M_sig_MR;
	tmp_W_sig_WR = M_sig_WR;
}
