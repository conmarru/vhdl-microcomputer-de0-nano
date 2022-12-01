#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define OPCODE_SHIFT_VAL 27
#define DEST_REG_SHIFT_VAL 22
#define OP1_REG_SHIFT_VAL 17
#define OP2_REG_SHIFT_VAL 12

struct memory_node {
	struct memory_node* next;
	uint32_t addr;
	int32_t val;
};

static int32_t regs[32] = { 0 };

static struct memory_node head = {NULL, 0, 0};

struct memory_node* new_memory_node(struct memory_node* next, uint32_t addr, int32_t val)
{	
	struct memory_node* new = (struct memory_node*)malloc(sizeof(struct memory_node));

	new->next = next;
	new->addr = addr;
	new->val = val;

	return new;
}

int32_t load_from_mem(uint32_t addr)
{
	struct memory_node* node = head.next;

	while (node)
	{
		if (node->addr == addr)
		{
			return node->val;
		}
		node = node->next;
	}

	return 0;
}

void write_to_mem(uint32_t addr, int32_t val)
{
	struct memory_node* node = head.next;
	
	if (!node)
	{
		struct memory_node* new = new_memory_node(NULL, addr, val);
		head.next = new;
		return;
	}

	while (node->next)
	{
		if (node->addr == addr)
		{
			node->val = val;
			return;
		}
		node = node->next;
	}
	
	if (node->addr == addr)
	{
		node->val = val;
		return;
	}

	struct memory_node* new = new_memory_node(NULL, addr, val);
	node->next = new;
}

void write_reg(uint8_t reg_num, int32_t val)
{
	if (reg_num >= 32)
	{
		return;
	}	
	regs[reg_num] = val;
}

int32_t read_reg(uint8_t reg_num)
{
	return (reg_num < 32) ? regs[reg_num] : 0;
}

void sim_instr(uint32_t instr)
{
	uint8_t opcode = instr >> OPCODE_SHIFT_VAL;
	uint8_t dest_reg = (instr >> DEST_REG_SHIFT_VAL) & 0x0000001f;
	uint8_t op1_reg = (instr >> OP1_REG_SHIFT_VAL) & 0x0000001f;
	uint8_t op2_reg = (instr >> OP2_REG_SHIFT_VAL) & 0x0000001f;
	uint8_t func = instr & 0x0000001f;
	int32_t immediate;

	switch (opcode)
	{
		case 2: regs[dest_reg] = regs[op1_reg];
			break;
		case 3: immediate = instr & 0x003fffff;
			if (immediate >> 21) immediate |= 0xffc00000;
			regs[dest_reg] = immediate;
			break;
		case 4: switch (func)
			{
				case 0: regs[dest_reg] = regs[op1_reg] + regs[op2_reg];
					break;
				case 1: regs[dest_reg] = regs[op1_reg] >> regs[op2_reg];
					break;
				case 2: regs[dest_reg] = regs[op1_reg] << regs[op2_reg];
					break;
				case 3: regs[dest_reg] = regs[op1_reg] & regs[op2_reg];
					break;
				case 4: regs[dest_reg] = regs[op1_reg] | regs[op2_reg];
					break;
			}
			break;
		case 5: immediate = instr & 0x00007fff;
			if (immediate >> 16) immediate |= 0xffff8000;
			regs[dest_reg] = regs[op1_reg] + immediate;
			break;
		case 8: regs[dest_reg] = load_from_mem(regs[29]);
			break;
		case 9: write_to_mem(regs[29], regs[dest_reg]);
			break;
		case 10: regs[dest_reg] = ~regs[op1_reg];
			 break;
	}
}


