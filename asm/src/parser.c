#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include "../headers/scan.h"
#include "../headers/def.h"
#include "../headers/utils.h"
#include "../headers/lbl.h"
#include "../headers/eval.h"
#include "../headers/gen.h"

static const uint8_t STACK_POINTER_REG = 30;
static const uint8_t ADDR_REG = 29;

static uint32_t cur_addr = 0;
static int dyn_instr_counter = 0;

typedef enum {
	REG_OP1,
	REG_OP2,
	REG_DEST
} reg_type;

typedef enum {
	DYN_INSTR_ADD,
	DYN_INSTR_ADDI,
	DYN_INSTR_LOADI,
	DYN_INSTR_SHL,
	DYN_INSTR_SHLB,
	DYN_INSTR_NOT,
	DYN_INSTR_LOAD,
	DYN_INSTR_STORE,
	DYN_INSTR_JMP,
	DYN_INSTR_BRE,
	DYN_INSTR_AND,
	DYN_INSTR_SHR,
	DYN_INSTR_MOVE,
	DYN_INSTR_NOP
} dyn_instr_type;

struct dyn_instr_struct {
	dyn_instr_type ditype;
	uint8_t reg_op1;
	uint8_t reg_op2;
	uint8_t reg_dest;
	int32_t num_val;
};

struct keyword_node {
	struct keyword_node* next;
	char* keyword;
	uint8_t opcode;
	uint8_t func;
};

struct reg_struct {
	char* name;
	uint8_t encoding;
};

struct reg_struct_node {
	struct reg_struct_node* next;
	struct reg_struct* reg;	
};


static struct keyword_node* keyword_hashset[KEYWORD_HASHSET_LEN] = { NULL };
static struct reg_struct_node* reg_hashset[REG_NUM] = { NULL };

static unsigned int instr_list_len = 50;
unsigned int instr_list_ptr = 0;
struct instr_struct** instr_list = NULL;

static struct instr_struct* new_instr_struct(int32_t encoding, bool jmp_tag, uint32_t instr_addr, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, uint32_t immediate, uint8_t func)
{
	struct instr_struct* new = (struct instr_struct*)malloc(sizeof(struct instr_struct));

	new->jmp_tag = jmp_tag;
	new->encoding = encoding;
	new->instr_addr = instr_addr;
	new->opcode = opcode;
	new->dest = dest;
	new->op1 = op1;
	new->op2 = op2;
	new->immediate = immediate;
	new->func = func;

	return new;
}

static struct dyn_instr_struct* new_dyn_instr_struct(dyn_instr_type ditype, uint8_t op1, uint8_t op2, uint8_t dest, int32_t num)
{
	struct dyn_instr_struct* new = (struct dyn_instr_struct*)malloc(sizeof(struct dyn_instr_struct));
	
	new->ditype = ditype;
	new->reg_op1 = op1;
	new->reg_op2 = op2;
	new->reg_dest = dest;
	new->num_val = num;

	return new;
}

static struct reg_struct* new_reg_struct(char* name, uint8_t encoding)
{
	struct reg_struct* new = (struct reg_struct*)malloc(sizeof(struct reg_struct));
	new->name = name; 
	new->encoding = encoding;

	return new;
}

static struct reg_struct_node* new_reg_struct_node(struct reg_struct_node* next, struct reg_struct* reg)
{
	struct reg_struct_node* new = (struct reg_struct_node*)malloc(sizeof(struct reg_struct_node));
	new->next = next;
	new->reg = reg;

	return new;
}

static void free_reg_struct_node(struct reg_struct_node* node)
{
	free(node->reg);
	free(node);
}

static void free_reg_hashset()
{
	for (int i = 0; i < REG_NUM; i++)
	{
		struct reg_struct_node* head = reg_hashset[i];
		while (head->next)
		{
			struct reg_struct_node* node = head->next;
			head->next = node->next;	
			free_reg_struct_node(node);	
		}
		free_reg_struct_node(head);
	}
}

static void add_instr_struct(uint32_t encoding, bool jmp_flag, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, int32_t immediate, uint8_t func)
{
	if (instr_list_ptr == instr_list_len-1)
	{
		instr_list = realloc(instr_list, instr_list_len*2);
	}
	instr_list[instr_list_ptr++] = new_instr_struct(encoding, jmp_flag, cur_addr, opcode, dest, op1, op2, immediate, func);
}

void free_instr_structs()
{
	for (unsigned int i = 0; i <= instr_list_ptr; i++)
	{
		if (instr_list[i]) free(instr_list[i]);
	}
}

static void update_addrs()
{
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{	
		if (instr_list[i]->jmp_tag)
		{
			if (instr_list[i]->opcode == 3 && (instr_list[i]->instr_addr) <= cur_addr && instr_list[i]->immediate > cur_addr)
			{
				instr_list[i]->immediate++;
			}	
			else if (instr_list[i]->opcode == 7)
			{
				int32_t offset = instr_list[i]->immediate; 
				uint32_t target_addr = instr_list[i]->instr_addr + offset + 1;
				if (instr_list[i]->instr_addr < cur_addr && target_addr > cur_addr)
				{
					offset += 1;
					instr_list[i]->immediate++;
				}
			}
		}
	}
}

static void add_reg(char* name, uint8_t encoding)
{
	struct reg_struct_node* head = reg_hashset[get_hash(name, REG_NUM)];	
	struct reg_struct* reg = new_reg_struct(name, encoding);
	if (!head->next)
	{
		head->next = new_reg_struct_node(NULL, reg);
		return;
	}
	struct reg_struct_node* new = new_reg_struct_node(head->next, reg);
	head->next = new;
}

struct reg_struct* get_reg_struct(char* name)
{
	struct reg_struct_node* node = reg_hashset[get_hash(name, REG_NUM)]->next;

	while (node)
	{
		if (!strcmp(node->reg->name, name))
		{
			return node->reg;
		}

		node = node->next;
	}

	return NULL;
}

static void init_reg_hashset()
{
	for (int i = 0; i < REG_NUM; i++)
	{
		reg_hashset[i] = new_reg_struct_node(NULL, NULL);
	}

	add_reg("r0", 0);
	add_reg("r1", 1);
	add_reg("r2", 2);
	add_reg("r3", 3);
	add_reg("r4", 4);
	add_reg("r5", 5);
	add_reg("r6", 6);
	add_reg("r7", 7);
	add_reg("r8", 8);
	add_reg("r9", 9);
	add_reg("r10", 10);
	add_reg("r11", 11);
	add_reg("r12", 12);
	add_reg("r13", 13);
	add_reg("r14", 14);
	add_reg("r15", 15);
	add_reg("r16", 16);
	add_reg("r17", 17);
	add_reg("r18", 18);
	add_reg("r19", 19);
	add_reg("r20", 20);
	add_reg("r21", 21);
	add_reg("r22", 22);
	add_reg("r23", 23);
	add_reg("r24", 24);
	add_reg("r25", 25);
	add_reg("r26", 26);
	add_reg("r27", 27);
	add_reg("r28", 28);
	add_reg("r29", 29);
	add_reg("r30", 30);
	add_reg("r31", 31);
}

static struct keyword_node* new_keyword_node(struct keyword_node* next, char* keyword, uint8_t opcode, uint8_t func)
{
	struct keyword_node* new = (struct keyword_node*)malloc(sizeof(struct keyword_node));
	new->next = next;
	new->keyword = keyword;
	new->opcode = opcode;
	new->func = func;

	return new;
}

static void free_keyword_hashset()
{
	for (int i = 0; i < KEYWORD_HASHSET_LEN; i++)
	{
		struct keyword_node* head = keyword_hashset[i];
		while (head->next)
		{
			struct keyword_node* knode = head->next;
			head->next = knode->next;
			free(knode);
		}
		free(head);
	}
}

static void add_keyword(char* keyword, uint8_t opcode, uint8_t func)
{
	struct keyword_node* head = keyword_hashset[get_hash(keyword, KEYWORD_HASHSET_LEN)];

	if (!head->next)
	{
		head->next = new_keyword_node(NULL, keyword, opcode, func);
		return;
	}
	struct keyword_node* new = new_keyword_node(head->next, keyword, opcode, func);
	head->next = new;
}

static struct keyword_node* get_keyword_node(char* keyword)
{
	struct keyword_node* node = keyword_hashset[get_hash(keyword, KEYWORD_HASHSET_LEN)]->next;

	while (node)
	{
		if (!strcmp(node->keyword, keyword))
		{
			return node;
		}

		node = node->next;
	}

	return NULL;
}

static void init_keyword_hashset()
{
	for (int i = 0; i < KEYWORD_HASHSET_LEN; i++)
	{
		keyword_hashset[i] = new_keyword_node(NULL, NULL, 0, 0);
	}
	
	// ISA instructions
	add_keyword("nop", 0, 0);
	add_keyword("hlt", 1, 0);
	add_keyword("move", 2, 0);
	add_keyword("loadi", 3, 0);
	add_keyword("add", 4, 0);
	add_keyword("shr", 4, 1);
	add_keyword("shl", 4, 2);
	add_keyword("shrb", 4, 7);
	add_keyword("shlb", 4, 8);
	add_keyword("and", 4, 3);
	add_keyword("or", 4, 4);
	add_keyword("not", 4, 5);
	add_keyword("addi", 5, 0);
	add_keyword("bre", 7, 0);
	add_keyword("brl", 7, 1);
	add_keyword("brg", 7, 2);
	
	// instructions that decode into mutliple instructions
	add_keyword("jmp", 6, 0);	
	add_keyword("load", 8, 0);
	add_keyword("store", 9, 0);

	// pseudoinstructions
	add_keyword("sub", 32, 0);
	add_keyword("push", 33, 0);
	add_keyword("pop", 34, 0);
	add_keyword("call", 35, 0);
	add_keyword("ret", 36, 0);
	add_keyword("mul", 37, 0);
}

static uint8_t parse_reg()
{
	token* t = consume_token();
	if (!expect(t, TOKEN_PERCENT))
	{
		error_unexpected_token(TOKEN_PERCENT, t);
	}
	free_token(t);

	t = consume_token();
	if (!expect(t, TOKEN_IDNT))
	{
		error_unexpected_token(TOKEN_IDNT, t);
	}

	struct reg_struct* reg = get_reg_struct(t->value);
	if (!reg)
	{
		printf("ERROR: Invalid register '%s' at line %d\n", t->value, t->line_num);
		exit(1);
	}
	free_token(t);

	return reg->encoding;
}

static void parse_comma()
{
	token* t = consume_token();
	if (!expect(t, TOKEN_COMMA))
	{
		error_unexpected_token(TOKEN_COMMA, t);
	}
	free_token(t);
}

static void parse_newline(bool lbl)
{
	token* t = consume_token();
	if (!expect(t, TOKEN_NEWLINE))
	{
		error_unexpected_token(TOKEN_NEWLINE, t);
	}
	free_token(t);	
	if (!lbl) cur_addr++;
	while (get_current_token() && get_current_token()->type == TOKEN_NEWLINE)
	{
		consume_token();
	}
}

static int32_t parse_num(bool branch)
{
	uint32_t immediate_val;
	bool label_flag = false;
	bool sign_flag = false;
	token* t = consume_token();
	if (t->type == TOKEN_IDNT)
	{
		label* lbl = get_label(t->value);
		if (!lbl)
		{
			printf("ERROR: Referenced undefined label\n");
			exit(1);
		}
		immediate_val = lbl->addr;
		label_flag = true;
	}
	else if (t->type == TOKEN_LPAREN)
	{
		struct ast_node* root = parse_expr();
		immediate_val = eval(root);
		t = consume_token();
		if (!expect(t, TOKEN_RPAREN))
		{
			error_unexpected_token(TOKEN_RPAREN, t);
		}
		// TODO: free ast
	}
	else
	{

		if (!expect(t, TOKEN_DOLLAR))
		{
			error_unexpected_token(TOKEN_DOLLAR, t);
		}
		free_token(t);

		t = consume_token();
		if (check_ttype(t, TOKEN_MINUS))
		{
			sign_flag = true;
			t = consume_token();
		}

		if (!expect(t, TOKEN_NUM))
		{
			error_unexpected_token(TOKEN_NUM, t);
		}

		immediate_val = (sign_flag) ? -t->num_value : t->num_value;
	}
	if (branch)
	{
		if (label_flag)
		{
			int dif = immediate_val - cur_addr;
			immediate_val = (dif < 0) ? immediate_val - cur_addr : immediate_val - cur_addr - 1;
		}
	}
	free_token(t);

	immediate_val &= 0x0001ffff;
	return (int32_t)immediate_val;
}

static void gen_opcode4_instr(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	uint8_t dest;
	uint8_t op1;
	uint8_t op2 = 0;

	op1 = parse_reg();

	parse_comma();

	if (knode->func == 0 || knode->func == 3 || knode->func == 4)
	{
		op2 = parse_reg();

		parse_comma();
	}

	dest = parse_reg();

	add_instr_struct(bin_instr, false, knode->opcode, dest, op1, op2, 0, knode->func);

	parse_newline(false);
}

static void gen_move_instr(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	uint8_t dest;
	uint8_t op1;

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;

	op1 = parse_reg();

	parse_comma();

	dest = parse_reg();

	add_instr_struct(bin_instr, false, knode->opcode, dest, op1, 0, 0, 0);

	parse_newline(false);
}

static void gen_loadi(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	int32_t immediate;
	uint8_t dest;

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;

	immediate = parse_num(false);

	parse_comma();

	dest = parse_reg();

	add_instr_struct(bin_instr, false, knode->opcode, dest, 0, 0, immediate, 0);

	parse_newline(false);
}

static void gen_addi(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	int32_t immediate;
	uint8_t dest;
	uint8_t op1;

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;

	immediate = parse_num(false);

	parse_comma();

	op1 = parse_reg();

	parse_comma();

	dest = parse_reg();

	add_instr_struct(bin_instr, false, knode->opcode, dest, op1, 0, immediate, 0);

	parse_newline(false);

}

static void gen_dynamic_instr(struct dyn_instr_struct* dyn_instr, bool jmp_tag, bool first_flag)
{
	uint32_t bin_instr = 0;
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
	dyn_instr_counter++;
	add_instr_struct(bin_instr, jmp_tag, opcode, dest, op1, op2, immediate, func);
	if (!first_flag) 
	{
		update_lbls(cur_addr);
		update_addrs();
		cur_addr += 1;
	}
}

static void insert_nops(unsigned int amount)
{
	for (unsigned int i = 0; i < amount; i++)
	{
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_NOP, 0, 0, 0, 0), false, false);
	}
}

// r29,r28 reserved for jump addresses and scaling
static void gen_jump(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	uint32_t scale_reg = 0;
	uint32_t immediate = 0;
	uint32_t dest_reg = ADDR_REG;
	bool scaling = false;

	token* t = get_current_token();
	if (check_ttype(t, TOKEN_PERCENT))
	{
		dest_reg = parse_reg();	
	}
	// no scaling
	else if ((get_current_token() && (get_current_token()->type == TOKEN_DOLLAR || get_current_token()->type == TOKEN_IDNT)))
	{
		immediate = parse_num(false);	
	}
	else
	{
		scaling = true;
		scale_reg = parse_reg();

		parse_comma();

		immediate = parse_num(false);
	}
	
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, immediate), true, false);
	if (!(cli_flags & NO_NOPS_FLAG))
	{
		insert_nops(4);	
	}
	if (scaling) 
	{	
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHLB, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHLB, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);

		if (!(cli_flags & NO_NOPS_FLAG))
		{
			insert_nops(4);	
		}

		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0), false, false);
	}

	add_instr_struct(bin_instr, false, knode->opcode, dest_reg, 0, 0, 0, 0);

	if (!(cli_flags & NO_NOPS_FLAG))
	{
		insert_nops(3);	
	}

	parse_newline(false);

}

static void gen_branches(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	uint8_t dest;
	int32_t immediate;

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	bin_instr |= knode->func;

	dest = parse_reg();

	parse_comma();

	immediate = parse_num(true);

	add_instr_struct(bin_instr, true, knode->opcode, dest, 0, 0, immediate, knode->func);

	parse_newline(false);

}

static void gen_load(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;	
	uint32_t scale_reg = 0;
	uint32_t immediate = 0;
	uint32_t dest_reg;
	bool scaling = false;

	if (get_current_token() && get_current_token()->type == TOKEN_PERCENT)
	{
		scaling = true;
		scale_reg = parse_reg();
		parse_comma();
	}

	immediate = parse_num(false);

	parse_comma();

	dest_reg = parse_reg();

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, immediate), false, false);
	if (!(cli_flags & NO_NOPS_FLAG))
	{
		insert_nops(4);	
	}
	if (scaling)
	{
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHLB, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHLB, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		if (!(cli_flags & NO_NOPS_FLAG))
		{
			insert_nops(4);	
		}
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0), false, false);
	}

	add_instr_struct(bin_instr, false, knode->opcode, dest_reg, ADDR_REG, 0, 0, 0);

	parse_newline(false);
}

static void gen_store(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;	
	bool scaling = false;
	uint32_t scale_reg = 0;
	uint32_t src_reg = 0;
	uint32_t immediate = 0;

	src_reg = parse_reg();

	parse_comma();
	
	if (get_current_token() && get_current_token()->type == TOKEN_PERCENT)
	{
		scaling = true;
		scale_reg = parse_reg();
		parse_comma();
	}

	immediate = parse_num(false);

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, immediate), false, false);
	if (!(cli_flags & NO_NOPS_FLAG))
	{
		insert_nops(4);	
	}
	if (scaling)
	{
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHLB, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHLB, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, 0, scale_reg, 0), false, false);
		if (!(cli_flags & NO_NOPS_FLAG))
		{
			insert_nops(4);
		}
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0), false, false);
	}

	add_instr_struct(bin_instr, false, knode->opcode, ADDR_REG, src_reg, 0, 0, 0);

	parse_newline(false);

}

static void gen_pseudo_sub()
{
	uint8_t reg_op1 = parse_reg();

	parse_comma();

	uint8_t reg_op2 = parse_reg();

	parse_comma();

	uint8_t reg_dest = parse_reg();

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_NOT, reg_op2, 0, reg_op2, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, reg_op2, 0, reg_op2, 1), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, reg_op1, reg_op2, reg_dest, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_NOT, reg_op2, 0, reg_op2, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, reg_op2, 0, reg_op2, 1), false, true);

	parse_newline(false);
}

static void gen_pseudo_push()
{
	uint8_t op1 = parse_reg();
	
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_STORE, op1, 0, STACK_POINTER_REG, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, -1), false, true);

	parse_newline(false);
}

static void gen_pseudo_pop()
{
	uint8_t op1 = parse_reg();
	
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, 1), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOAD, STACK_POINTER_REG, 0, op1, 0), false, true);

	parse_newline(false);
}

static void gen_pseudo_call()
{
	uint32_t addr = parse_num(false);

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 29, cur_addr+5), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_STORE, 29, 0, STACK_POINTER_REG, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, -1), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 29, addr+3), true, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_JMP, 0, 0, 29, 0), false, true);

	parse_newline(false);
}

static void gen_pseudo_ret()
{
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, STACK_POINTER_REG, 0, STACK_POINTER_REG, 1), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOAD, STACK_POINTER_REG, 0, 29, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_JMP, 0, 0, 29, 0), false, true);

	parse_newline(false);
}

static void gen_pseudo_mul()
{	
	uint32_t start_addr = cur_addr;

	uint8_t reg_op1 = parse_reg();

	parse_comma();

	uint8_t reg_op2 = parse_reg();

	parse_comma();

	uint8_t reg_dest = parse_reg();
	
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_MOVE, reg_op1, 0, 25, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_MOVE, reg_op2, 0, 24, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 26, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 28, 1), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_BRE, 0, 0, 24, 7), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_AND, 24, 28, 27, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_BRE, 0, 0, 27, 8), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, 26, 25, 26, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHR, 24, 0, 24, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, 25, 0, 25, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, 29, start_addr+3), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_JMP, 0, 0, 29, 0), false, false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_MOVE, 26, 0, reg_dest, 0), false, true);

	parse_newline(false);	
}

void parse()
{
	init_keyword_hashset();
	init_reg_hashset();
	instr_list = (struct instr_struct**)malloc(sizeof(struct instr_struct*)*instr_list_len);
	while (get_current_token())
	{
		token* current = consume_token();
		if (!expect(current, TOKEN_IDNT))
		{
			error_unexpected_token(TOKEN_IDNT, current);
		}
		struct keyword_node* knode = get_keyword_node(current->value);
		if (!knode)
		{
			if (current->type == TOKEN_IDNT && get_label(current->value))
			{
				parse_newline(true);	
				continue;
			}
			else if (current->type == TOKEN_IDNT) printf("Invalid instruction found at line %d\n", current->line_num);
			else printf("ERROR: Instruction expected at line %d\n", current->line_num);
			exit(1);
		}
		switch (knode->opcode)
		{
			case 0: 
			case 1: uint32_t bin_instr = 0; 
				bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
				add_instr_struct(bin_instr, false, knode->opcode, 0, 0, 0, 0, 0);	
				parse_newline(false);
				break;
			case 2: gen_move_instr(knode);
				break;
			case 3: gen_loadi(knode);
				break;
			case 4: gen_opcode4_instr(knode);
				break;
			case 5: gen_addi(knode);
				break;
			case 6: gen_jump(knode);
				break;
			case 7: gen_branches(knode);
				break;
			case 8: gen_load(knode);
				break;
			case 9: gen_store(knode);
				break;
			case 32: gen_pseudo_sub();
				 break;
			case 33: gen_pseudo_push();
				 break;
			case 34: gen_pseudo_pop();
				 break;
			case 35: gen_pseudo_call();
				 break;
			case 36: gen_pseudo_ret();
				 break;
			case 37: gen_pseudo_mul();
				 break;
			default : printf("ERROR: Invalid opcode parsed\n");
				  exit(1);
		}		
	}
	free_reg_hashset();
	free_keyword_hashset();
	free_lbl_hashset();
}
