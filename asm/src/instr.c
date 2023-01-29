#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../headers/def.h"
#include "../headers/instr.h"
#include "../headers/lbl.h"

#define JMP_NOPS_NUM 3

#define IS_PSEUDO_INSTR(opcode) (opcode > 31)
#define INSERT_NOP_AT_INDEX(index) (add_instr_struct_at_index(false, 0, 0, 0, 0, 0, 0, index, false, false, NULL))

uint32_t cur_addr = 0;
unsigned int instr_list_len = 50;
unsigned int instr_list_ptr = 0;
struct instr_struct** instr_list = NULL;

static int ref_count[REG_NUM] = { 0 };

const uint8_t ADDR_REG = 29;
const uint8_t STACK_POINTER_REG = 30;

struct instr_struct* new_instr_struct(bool jmp_tag, uint32_t instr_addr, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, uint32_t immediate, uint8_t func, bool mltp_tag)
{
	struct instr_struct* new = (struct instr_struct*)malloc(sizeof(struct instr_struct));

	new->jmp_tag = jmp_tag;
	new->mltp_tag = mltp_tag;
	new->instr_addr = instr_addr;
	new->opcode = opcode;
	new->dest = dest;
	new->op1 = op1;
	new->op2 = op2;
	new->immediate = immediate;
	new->func = func;
	new->jmp_target = NULL;

	return new;
}

void free_instr_structs()
{
	for (unsigned int i = 0; i <= instr_list_ptr; i++)
	{
		if (instr_list[i]) free(instr_list[i]);
	}
}

void add_instr_struct(bool jmp_flag, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, int32_t immediate, uint8_t func, bool mltp_tag)
{
	if (instr_list_ptr == instr_list_len-1)
	{
		instr_list = realloc(instr_list, sizeof(struct instr_struct*)*instr_list_len*2);
		instr_list_len = instr_list_len*2;
	}
	instr_list[instr_list_ptr++] = new_instr_struct(jmp_flag, cur_addr, opcode, dest, op1, op2, immediate, func, mltp_tag);
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

void add_instr_struct_at_index(bool jmp_flag, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, int32_t immediate, uint8_t func, uint32_t addr, bool overwrite, bool mltp_flag, struct instr_struct* jmp_target)
{
	if (instr_list_ptr == instr_list_len-1)
	{
		instr_list = realloc(instr_list, sizeof(struct instr_struct*)*instr_list_len*2);
		instr_list_len = instr_list_len*2;
	}

	unsigned int rpointer = instr_list_ptr;

	if (!overwrite)
	{
		while (rpointer >= addr+1)
		{
			instr_list[rpointer] = instr_list[rpointer-1];
			instr_list[rpointer]->instr_addr++;
			rpointer--;
		}
		instr_list_ptr++;
	}
	
	/*
	 * if some instruction's jump target is a pseudo instruction that is about to be resolved
	 * change pointers such that said jump target now points to the proper instruction. 
	 * TODO: if two or more jmp targets point to the same instruction we must sore all p_from pointers
	 * in a proper data structure (e.g. a linked list)
	 */
	if (overwrite && instr_list[addr]->p_from)
	{
		struct instr_struct* old_instr = instr_list[addr];	
		instr_list[addr] = new_instr_struct(jmp_flag, addr, opcode, dest, op1, op2, immediate, func, mltp_flag);
		instr_list[addr]->jmp_target = jmp_target;
		old_instr->p_from->jmp_target = instr_list[addr];
		instr_list[addr]->p_from = old_instr->p_from;
	}
	else
	{
		instr_list[addr] = new_instr_struct(jmp_flag, addr, opcode, dest, op1, op2, immediate, func, mltp_flag);
		instr_list[addr]->jmp_target = jmp_target;
	}

}

static void gen_dynamic_instr(struct dyn_instr_struct* dyn_instr, bool jmp_tag, bool first_flag, uint32_t addr, struct instr_struct* jmp_target)
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
	add_instr_struct_at_index(jmp_tag, opcode, dest, op1, op2, immediate, func, addr, first_flag, false, jmp_target);	
}

// disgusting function
// TODO: probably refactor?
void resolve_instr(struct instr_struct* instr)
{
	switch (instr->opcode)
	{
		case 6: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, instr->dest, instr->jmp_target->instr_addr), true, true, instr->instr_addr, instr->jmp_target);
			gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_JMP, 0, 0, ADDR_REG, 0), false, false, instr->instr_addr+1, NULL);
			break;
		case 8: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, instr->immediate), false, true, instr->instr_addr, NULL);
			gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOAD, instr->op1, 0, instr->dest, 0), false, false, instr->instr_addr+1, NULL);
			break;
		case 9: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, instr->immediate), false, true ,instr->instr_addr, NULL);
			gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_STORE, instr->op1, 0, instr->dest, 0), false, false, instr->instr_addr+1, NULL);
			break;
		case 32: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_NOT, instr->op2, 0, instr->op2, 0), false, true, instr->instr_addr, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, instr->op2, 0, instr->op2, 1), false, false, instr->instr_addr+1, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, instr->op1, instr->op2, instr->dest, 0), false, false, instr->instr_addr+2, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_NOT, instr->op2, 0, instr->op2, 0), false, false, instr->instr_addr+3, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, instr->op2, 0, instr->op2, 1), false, false, instr->instr_addr+4, NULL);
			 break;
		case 33: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_STORE, instr->op1, 0, STACK_POINTER_REG, 0), false, true, instr->instr_addr, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, -1), false, false, instr->instr_addr+1, NULL);
			 break;
		case 34: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, 1), false, true, instr->instr_addr, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOAD, STACK_POINTER_REG, 0, instr->op1, 0), false, false, instr->instr_addr+1, NULL);
			 break;
		case 35: struct instr_struct* call_pointer = instr->jmp_target;
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, instr->instr_addr+5), true, true, instr->instr_addr, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_STORE, ADDR_REG, 0, STACK_POINTER_REG, 0), false, false, instr->instr_addr+1, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, -1), false, false, instr->instr_addr+2, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, call_pointer->instr_addr), true, false, instr->instr_addr+3, call_pointer);
			 call_pointer->p_from = instr_list[instr->instr_addr+3];
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_JMP, 0, 0, ADDR_REG, 0), false, false, instr->instr_addr+4, NULL);
			 instr_list[instr->instr_addr]->jmp_target = instr_list[instr->instr_addr+5];
			 instr_list[instr->instr_addr+5]->p_from = instr_list[instr->instr_addr];
		         break;
		case 36: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, 1), false, true, instr->instr_addr, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOAD, STACK_POINTER_REG, 0, 29, 0), false, false, instr->instr_addr+1, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_JMP, 0, 0, ADDR_REG, 0), false, false, instr->instr_addr+2, NULL);
			 break;
		case 37: gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_MOVE, instr->op1, 0, 25, 0), false, true, instr->instr_addr, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_MOVE, instr->op2, 0, 24, 0), false, false, instr->instr_addr+1, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 26, 0), false, false, instr->instr_addr+2, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 28, 1), false, false, instr->instr_addr+3, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_BRE, 0, 0, 24, 8), true, false, instr->instr_addr+4, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_AND, 24, 28, 27, 0), false, false, instr->instr_addr+5, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_BRE, 0, 0, 27, 6), true, false, instr->instr_addr+6, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, 26, 25, 26, 0), false, false, instr->instr_addr+7, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHR, 24, 0, 24, 0), false, false, instr->instr_addr+8, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, 25, 0, 25, 0), false, false, instr->instr_addr+9, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 29, instr->instr_addr+3), true, false, instr->instr_addr+10, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_JMP, 0, 0, 29, 0), false, false, instr->instr_addr+11, NULL);
			 gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_MOVE, 26, 0, instr->dest, 0), false, false, instr->instr_addr+12, NULL);
			 instr_list[instr->instr_addr+4]->jmp_target = instr_list[instr->instr_addr+12];
			 instr_list[instr->instr_addr+6]->jmp_target = instr_list[instr->instr_addr+12];
			 instr_list[instr->instr_addr+10]->jmp_target = instr_list[instr->instr_addr+3];
			 break;
	}

	free(instr);
}

static void adjust_jumps()
{
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{
		if (instr_list[i]->jmp_tag)
		{
			if (instr_list[i]->opcode == 3) 
			{
				instr_list[i]->immediate = instr_list[i]->jmp_target->instr_addr;
			}
			else
			{
				int dif = instr_list[i]->instr_addr - instr_list[i]->jmp_target->instr_addr;
				instr_list[i]->immediate = (dif < 0) ? -dif : dif;
				if (!(cli_flags & NO_BR_ADJ_FLAG)) instr_list[i]->immediate -= 4;
			}
		}
	}
}

// 1. create pointers to jump/branch targets
// 2. iterate through instruction list and resolve pseudo instructions
// 3. using the pointers adjust the target addresses of jumps and branches
void resolve_pseudo_instr()
{
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{
		if (instr_list[i]->jmp_tag)
		{
			if (instr_list[i]->opcode == 6 || instr_list[i]->opcode == 35)
			{
				instr_list[i]->jmp_target = instr_list[instr_list[i]->immediate];	
			}
			else
			{
				instr_list[i]->jmp_target = instr_list[instr_list[i]->instr_addr + instr_list[i]->immediate];
			}
			instr_list[i]->jmp_target->p_from = instr_list[i];
		}
	}

	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{
		if (IS_PSEUDO_INSTR(instr_list[i]->opcode) || instr_list[i]->mltp_tag)
		{
			resolve_instr(instr_list[i]);
		}
	}
	
	adjust_jumps();	
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
			INSERT_NOP_AT_INDEX(index);
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
			case 1: for (int j = 0; j < 4; j++)
				{
					INSERT_NOP_AT_INDEX(i);
					i++;
				}
				break;
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
					INSERT_NOP_AT_INDEX(i);
				}
				else
				{
					i++;
					for (int j = 0; j < JMP_NOPS_NUM; j++)
					{
						INSERT_NOP_AT_INDEX(i);		
					}
				}
				break;
			case 7: if (ref_count[instr_list[i]->dest])
				{
					INSERT_NOP_AT_INDEX(i);
				}
				else
				{
					i++;
					for (int j = 0; j < JMP_NOPS_NUM; j++)
					{
						INSERT_NOP_AT_INDEX(i);		
					}
				}
				break;
			case 8:	if (ref_count[instr_list[i]->op1])
				{
					INSERT_NOP_AT_INDEX(i);
				}
				else
				{
					ref_count[instr_list[i]->dest] = 5;
				}
				break;
			case 9: if (ref_count[instr_list[i]->op1])
				{
					INSERT_NOP_AT_INDEX(i);
				}
				else
				{
					ref_count[instr_list[i]->dest] = 5;
				}
				break;
		}
	}

	adjust_jumps();

	if (cli_flags & DEBUG_FLAG) debug_print_instr_list(false, 0, 0);
}

void debug_print_instr_list(bool filter, int opcode, int func)
{
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{
		struct instr_struct* target = instr_list[i]->jmp_target;
		int target_addr = -1;
		if (target) target_addr = target->instr_addr;

		if (filter)
		{	
			if (instr_list[i]->opcode == opcode && instr_list[i]->func == func) printf("i: %d, opcode: %d, op1: %d, op2: %d, dest: %d, imm: %d, func: %d, addr: %d, jmp_target: %d\n", i, instr_list[i]->opcode, instr_list[i]->op1, instr_list[i]->op2, instr_list[i]->dest, instr_list[i]->immediate, instr_list[i]->func, instr_list[i]->instr_addr, target_addr);
		}
		else
		{
			printf("i: %d, opcode: %d, op1: %d, op2: %d, dest: %d, imm: %d, func: %d, addr: %d, jmp_target: %d\n", i, instr_list[i]->opcode, instr_list[i]->op1, instr_list[i]->op2, instr_list[i]->dest, instr_list[i]->immediate, instr_list[i]->func, instr_list[i]->instr_addr, target_addr);
		}

	}
}
