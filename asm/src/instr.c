#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../headers/def.h"
#include "../headers/instr.h"
#include "../headers/lbl.h"

#define JMP_NOPS_NUM 3

uint32_t cur_addr = 0;
unsigned int instr_list_len = 50;
unsigned int instr_list_ptr = 0;
struct instr_struct** instr_list = NULL;

static int ref_count[REG_NUM] = { 0 };

struct instr_struct* new_instr_struct(bool jmp_tag, uint32_t instr_addr, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, uint32_t immediate, uint8_t func)
{
	struct instr_struct* new = (struct instr_struct*)malloc(sizeof(struct instr_struct));

	new->jmp_tag = jmp_tag;
	new->instr_addr = instr_addr;
	new->opcode = opcode;
	new->dest = dest;
	new->op1 = op1;
	new->op2 = op2;
	new->immediate = immediate;
	new->func = func;

	return new;
}

void free_instr_structs()
{
	for (unsigned int i = 0; i <= instr_list_ptr; i++)
	{
		if (instr_list[i]) free(instr_list[i]);
	}
}

void add_instr_struct(bool jmp_flag, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, int32_t immediate, uint8_t func)
{
	if (instr_list_ptr == instr_list_len-1)
	{
		instr_list = realloc(instr_list, sizeof(struct instr_struct*)*instr_list_len*2);
		instr_list_len = instr_list_len*2;
	}
	instr_list[instr_list_ptr++] = new_instr_struct(jmp_flag, cur_addr, opcode, dest, op1, op2, immediate, func);
}

struct dyn_instr_struct* new_dyn_instr_struct(dyn_instr_type ditype, uint8_t op1, uint8_t op2, uint8_t dest, int32_t num)
{
	struct dyn_instr_struct* new = (struct dyn_instr_struct*)malloc(sizeof(struct dyn_instr_struct));
	
	new->ditype = ditype;
	new->reg_op1 = op1;
	new->reg_op2 = op2;
	new->reg_dest = dest;
	new->num_val = num;

	return new;
}

static void update_addrs(bool use_addr, uint32_t index)
{
	uint32_t ucur_addr = (use_addr) ? cur_addr : index;
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{	

		if (instr_list[i]->jmp_tag)
		{
			if (instr_list[i]->opcode == 3 && instr_list[i]->immediate > ucur_addr)
			{
				instr_list[i]->immediate++;
			}	
			else if (instr_list[i]->opcode == 7)
			{
				int32_t offset = instr_list[i]->immediate; 
				uint32_t target_addr = instr_list[i]->instr_addr + offset;
				if (instr_list[i]->instr_addr < ucur_addr && target_addr > ucur_addr)
				{
					offset += 1;
					instr_list[i]->immediate++;
				}
			}
		}
	}
}

void gen_dynamic_instr(struct dyn_instr_struct* dyn_instr, bool jmp_tag, bool first_flag)
{
	uint8_t opcode = 0;
	uint8_t dest = 0;
	uint8_t op1 = 0;
	uint8_t op2 = 0;
	int32_t immediate = 0;
	uint8_t func = 0;

	switch (dyn_instr->ditype)
	{
		case DYN_INSTR_ADD : opcode = 4;
				     dest = dyn_instr->reg_dest;
				     op1 = dyn_instr->reg_op1;
				     op2 = dyn_instr->reg_op2;
				     func = 0;
				     break;
		case DYN_INSTR_ADDI: opcode = 5;
				     dest = dyn_instr->reg_dest;
				     op1 = dyn_instr->reg_op1;
				     immediate = dyn_instr->num_val;
				     break;
		case DYN_INSTR_LOADI: opcode = 3;
				      dest = dyn_instr->reg_dest;
				      immediate = dyn_instr->num_val;
				      break;
		case DYN_INSTR_SHL: opcode = 4;
				    dest = dyn_instr->reg_dest;
				    op1 = dyn_instr->reg_op1;
				    func = 2;
				    break;
		case DYN_INSTR_SHLB: opcode = 4;
				     dest = dyn_instr->reg_dest;
				     op1 = dyn_instr->reg_op1;
				     func = 8;
				     break;
		case DYN_INSTR_NOT: opcode = 4;
				    dest = dyn_instr->reg_dest;
				    op1 = dyn_instr->reg_op1;
				    func = 5;
				    break;
		case DYN_INSTR_LOAD: opcode = 8;
				     dest = dyn_instr->reg_dest;
				     op1 = dyn_instr->reg_op1;
				     break;
		case DYN_INSTR_STORE: opcode = 9;
				      dest = dyn_instr->reg_dest;
				      op1 = dyn_instr->reg_op1;
				      break;
		case DYN_INSTR_JMP: opcode = 6;
				    dest = dyn_instr->reg_dest;
				    break;
		case DYN_INSTR_BRE: opcode = 7;
				    dest = dyn_instr->reg_dest;
				    immediate = dyn_instr->num_val;
				    break;
		case DYN_INSTR_AND: opcode = 4;
				    dest = dyn_instr->reg_dest;
				    op1 = dyn_instr->reg_op1;
				    op2 = dyn_instr->reg_op2;
				    func = 3;
				    break;
		case DYN_INSTR_SHR: opcode = 4;
				    dest = dyn_instr->reg_dest;
				    op1 = dyn_instr->reg_op1;
				    func = 1;
				    break;
		case DYN_INSTR_MOVE: opcode = 2;
				     dest = dyn_instr->reg_dest;
				     op1 = dyn_instr->reg_op1;
				     break;
		case DYN_INSTR_NOP: opcode = 0;
				    break;
	}

	free(dyn_instr);
	add_instr_struct(jmp_tag, opcode, dest, op1, op2, immediate, func);
	if (!first_flag) 
	{
		update_lbls(cur_addr);
		update_addrs(true, 0);
		cur_addr += 1;
	}
}

void insert_nops(unsigned int amount)
{
	for (unsigned int i = 0; i < amount; i++)
	{
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_NOP, 0, 0, 0, 0), false, false);
	}
}

static void insert_nop_at_index(unsigned int index)
{
	if (instr_list_ptr == instr_list_len-1)
	{
		instr_list = realloc(instr_list, sizeof(struct instr_struct*)*instr_list_len*2);
		instr_list_len = instr_list_len*2;
	}

	unsigned int rpointer = instr_list_ptr;

	while (rpointer >= index+1)
	{
		instr_list[rpointer] = instr_list[rpointer-1];
		instr_list[rpointer]->instr_addr++;
		rpointer--;
	}

	instr_list[index] = new_instr_struct(false, index, 0, 0, 0, 0, 0, 0);
	instr_list_ptr++;
	update_addrs(false, index);	
}

static void decr_ref_count()
{
	for (int i = 0; i < REG_NUM; i++)
	{
		ref_count[i] = (ref_count[i] > 0) ? ref_count[i] - 1 : 0;
	}
}

static void gen_nops_arith(uint8_t dest, uint8_t op1, uint8_t op2, uint32_t index, bool unary)
{
	uint8_t op;
	if (unary)
	{
		op = op1;
	}
	else
	{
		op = (ref_count[op1] >= ref_count[op2]) ? op1 : op2;
	}

	if (ref_count[op])
	{
		for (int j = 0; j < ref_count[op]; j++)
		{
			insert_nop_at_index(index);
		}
	}
	else
	{
		ref_count[dest] = 5; // 5, since it gets decremented instantly the next iteration
	}
}

void gen_nops()
{
	for (uint32_t i = 0; i < instr_list_ptr; i++)
	{
		decr_ref_count();
		switch (instr_list[i]->opcode)
		{
			case 2: gen_nops_arith(instr_list[i]->dest, instr_list[i]->op1, 0, i, true);
				break;
			case 3: ref_count[instr_list[i]->dest] = 5;
				break;
			case 4: gen_nops_arith(instr_list[i]->dest, instr_list[i]->op1, instr_list[i]->op2, i, false);
				break;
			case 5: gen_nops_arith(instr_list[i]->dest, instr_list[i]->op1, 0, i, true); 
				break;			
			case 6:	if (ref_count[instr_list[i]->dest])
				{
					insert_nop_at_index(i);
				}
				else
				{
					i++;
					for (int j = 0; j < JMP_NOPS_NUM; j++)
					{
						insert_nop_at_index(i);		
					}
				}
				break;
			case 7: if (ref_count[instr_list[i]->dest])
				{
					insert_nop_at_index(i);
				}
				else
				{
					i++;
					for (int j = 0; j < JMP_NOPS_NUM; j++)
					{
						insert_nop_at_index(i);		
					}
				}
				break;
			case 9: ref_count[instr_list[i]->dest] = 5;
				break;
		}
	}
	//debug_print_instr_list(false, 0, 0);
}

void debug_print_instr_list(bool filter, int opcode, int func)
{
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{
		if (filter)
		{	
			if (instr_list[i]->opcode == opcode && instr_list[i]->func == func) printf("i: %d, opcode: %d, op1: %d, op2: %d, dest: %d, imm: %d, func: %d, addr: %d\n", i, instr_list[i]->opcode, instr_list[i]->op1, instr_list[i]->op2, instr_list[i]->dest, instr_list[i]->immediate, instr_list[i]->func, instr_list[i]->instr_addr);
		}
		else
		{
			printf("i: %d, opcode: %d, op1: %d, op2: %d, dest: %d, imm: %d, func: %d, addr: %d\n", i, instr_list[i]->opcode, instr_list[i]->op1, instr_list[i]->op2, instr_list[i]->dest, instr_list[i]->immediate, instr_list[i]->func, instr_list[i]->instr_addr);
		}

	}
}
