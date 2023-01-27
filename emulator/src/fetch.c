#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "../headers/defs.h"
#include "../headers/decode.h"
#include "../headers/fetch.h"

#define LAST_FIELD_MASK 0x0000001f

bool hlt_flag = false;

// TODO: hlt

static int32_t tmp_D_next_pc;
static int8_t tmp_D_opcode;
static int8_t tmp_D_dest;
static int8_t tmp_D_op1; 
static int8_t tmp_D_op2; 
static int32_t tmp_D_imm;
static int tmp_D_sig_I;  
static int tmp_D_sig_J;  
static int tmp_D_sig_B;  
static int8_t tmp_D_func;
static int tmp_D_sig_MR; 
static int tmp_D_sig_MW;
static int tmp_D_sig_WR; 

static unsigned int instr_mem_size;

struct extract_fields_struct {
	int32_t instr_in;
	int8_t opcode_out; // opcode
	int8_t dest_out; // reigster number for destination operand
	int8_t op1_out; // register number for first operand
	int8_t op2_out; // register number for second operand 
	int32_t imm_out; // immediate value
	int8_t func_out; // function 
	int sig_I_out; // immediate signal 
	int sig_J_out; // jump signal
	int sig_B_out; // branch signal
	int sig_MR_out; // read memory
	int sig_MW_out; // write memory
	int sig_WR_out; // write register
};

struct instr_mem_struct instr_mem;
struct extract_fields_struct extract_fields;
struct pc_struct pc_str;

static int32_t read_mem(struct instr_mem_struct* mem, uint32_t addr)
{	
	if (addr >= instr_mem_size)
	{
		printf("ERROR: Invalid memory access. Referenced address exceeded the bounds of memory\n");
		exit(1);
	}
	return mem->mem_array[(uint32_t)addr];
}

static void read_instr_mem(struct instr_mem_struct* mem, struct extract_fields_struct* extr_fields, struct pc_struct* pc_strct)
{
	mem->addr_in = pc_strct->pc_out;
	extr_fields->instr_in = read_mem(mem, mem->addr_in);
}

static void extract_field_info(struct extract_fields_struct* extr_fields)
{
	int32_t opcode = extr_fields->instr_in >> OPCODE_SHIFT_VAL;
	extr_fields->opcode_out = opcode;
	int32_t instr = extr_fields->instr_in;

	switch (opcode)
	{
		case 0: extr_fields->sig_I_out = 0;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 0;
			break; 
		case 1: if (flags & 32) hlt_flag = true;
			break; // TODO: hlt
		case 2: extr_fields->dest_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->op1_out = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->sig_I_out = 0;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 1;
			break;
		case 3: extr_fields->dest_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->imm_out = instr & IMM_17_MASK;
			extr_fields->sig_I_out = 1;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 1;
			break;
		case 4: extr_fields->dest_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->op1_out = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->op2_out = (instr >> OP2_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->func_out = instr & EXTRACT_MASK;
			extr_fields->sig_I_out = 0;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 1;
			break;
		case 5: extr_fields->dest_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->op1_out = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->imm_out = instr & IMM_17_MASK;
			extr_fields->sig_I_out = 1;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 1;
			break;
		case 6: extr_fields->dest_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->sig_J_out = 1;
			extr_fields->sig_I_out = 0;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 0;
			break;
		case 7: extr_fields->dest_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->imm_out = (instr & 0x003fffe0) >> 5;
			extr_fields->func_out = (instr & LAST_FIELD_MASK) & EXTRACT_MASK;
			extr_fields->sig_B_out = 1;
			extr_fields->sig_I_out = 1;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 0;
			break;
		case 8: extr_fields->dest_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->op1_out = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_I_out = 0;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_MR_out = 1;
			extr_fields->sig_MW_out = 0;
			extr_fields->sig_WR_out = 1;
			break;
		case 9: extr_fields->dest_out = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK; // flip source and dest
			extr_fields->op1_out = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
			extr_fields->sig_B_out = 0;
			extr_fields->sig_I_out = 0;
			extr_fields->sig_J_out = 0;
			extr_fields->sig_MR_out = 0;
			extr_fields->sig_MW_out = 1;
			extr_fields->sig_WR_out = 0;
			break;
	}
}

void update_pc(struct pc_struct* pc_strct)
{
	//pc = new_pc;
	if (pc_strct->used_jump_in || pc_strct->used_branch_in)
	{
		pc_strct->jmp_counter = 3;
		pc_strct->pc_out = pc_strct->next_pc_in;
		pc_strct->pc_temp = pc_strct->next_pc_in + 1;
		pc_strct->next_pc_out = pc_strct->next_pc_in + 4;
	}
	else if (pc_strct->jmp_counter > 0)
	{
		pc_strct->jmp_counter--;
		pc_strct->pc_out = pc_strct->pc_temp;
		pc_strct->next_pc_out = pc_strct->pc_temp + 4;
		pc_strct->pc_temp = pc_strct->pc_temp + 1;
	}
	else
	{
		pc_strct->pc_out = pc_strct->next_pc_in;
		pc_strct->next_pc_out = pc_strct->next_pc_out + 1;
	}
}

void alloc_instr_mem(unsigned int size)
{
	instr_mem_size = size;
	instr_mem.mem_array = (int32_t*)malloc(sizeof(int32_t)*size);
}

void free_instr_mem()
{
	if (instr_mem.mem_array) free(instr_mem.mem_array);
}

void fetch_update_pipeline_regs()
{
	D_next_pc = tmp_D_next_pc;
	D_opcode = tmp_D_opcode;
        D_dest = tmp_D_dest;
        D_op1 = tmp_D_op1;
        D_op2 = tmp_D_op2;
        D_imm = tmp_D_imm;
        D_sig_I = tmp_D_sig_I;
        D_sig_J = tmp_D_sig_J;
        D_sig_B = tmp_D_sig_B;
        D_func = tmp_D_func;
        D_sig_MR = tmp_D_sig_MR;
        D_sig_MW = tmp_D_sig_MW;
        D_sig_WR = tmp_D_sig_WR;
}

void fetch()
{
	read_instr_mem(&instr_mem, &extract_fields, &pc_str);
	extract_field_info(&extract_fields);
	tmp_D_next_pc = pc_str.next_pc_out;
	tmp_D_opcode = extract_fields.opcode_out;
        tmp_D_dest = extract_fields.dest_out;
        tmp_D_op1 = extract_fields.op1_out;
        tmp_D_op2 = extract_fields.op2_out;
        tmp_D_imm = extract_fields.imm_out;
        tmp_D_sig_I = extract_fields.sig_I_out;
        tmp_D_sig_J = extract_fields.sig_J_out;
        tmp_D_sig_B = extract_fields.sig_B_out;
        tmp_D_func = extract_fields.func_out;
        tmp_D_sig_MR = extract_fields.sig_MR_out;
        tmp_D_sig_MW = extract_fields.sig_MW_out;
        tmp_D_sig_WR = extract_fields.sig_WR_out;
}
