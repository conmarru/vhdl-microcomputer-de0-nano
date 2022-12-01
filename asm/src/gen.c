#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include "../headers/scan.h"
#include "../headers/def.h"
#include "../headers/utils.h"
#include "../headers/regs.h"

#define KEYWORD_HASHSET_LEN 20
#define REG_NUM 32
#define OPCODE_SHIFT_VAL 27
#define DEST_REG_SHIFT_VAL 22
#define OP1_REG_SHIFT_VAL 17
#define OP2_REG_SHIFT_VAL 12
#define BR_FUNC_SHIFT_VAL 19
#define IMM_22 22
#define IMM_17 17
#define OFFSET_19 19
#define ADDR_18 18
#define ADDR_22 22

static const uint8_t ADDR_REG = 29;
static const uint8_t SCALE_REG = 28;

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
	DYN_INSTR_NOT
} dyn_instr_type;

struct dyn_instr_struct {
	dyn_instr_type ditype;
	uint8_t reg_op1;
	uint8_t reg_op2;
	uint8_t reg_dest;
	uint32_t num_val;
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

struct instr_struct {
	bool jmp_tag;
	uint32_t encoding;
	uint32_t instr_addr;
};

static struct keyword_node* keyword_hashset[KEYWORD_HASHSET_LEN] = { NULL };
static struct reg_struct_node* reg_hashset[REG_NUM] = { NULL };

static unsigned int instr_list_len = 50;
static unsigned int instr_list_ptr = 0;
struct instr_struct** instr_list = NULL;

static struct instr_struct* new_instr_struct(int32_t encoding, bool jmp_tag, uint32_t instr_addr)
{
	struct instr_struct* new = (struct instr_struct*)malloc(sizeof(struct instr_struct));

	new->jmp_tag = jmp_tag;
	new->encoding = encoding;
	new->instr_addr = instr_addr;

	return new;
}

static struct dyn_instr_struct* new_dyn_instr_struct(dyn_instr_type ditype, uint8_t op1, uint8_t op2, uint8_t dest, uint32_t num)
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

static void add_instr_struct(uint32_t encoding, bool jmp_flag)
{
	if (instr_list_ptr == instr_list_len-1)
	{
		instr_list = realloc(instr_list, instr_list_len*2);
	}
	instr_list[instr_list_ptr++] = new_instr_struct(encoding, jmp_flag, cur_addr);
}

static void update_addrs()
{
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{	
		if (instr_list[i]->jmp_tag)
		{
			// correct jmp
			if ((instr_list[i]->instr_addr) <= cur_addr && (instr_list[i]->encoding & 0x003fffff) > instr_list[i]->instr_addr)
			{
				uint32_t new_val = (instr_list[i]->encoding & 0x003fffff) + 4;
				instr_list[i]->encoding = (instr_list[i]->encoding & 0xffc00000) | new_val;
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
	add_keyword("and", 4, 3);
	add_keyword("or", 4, 4);
	add_keyword("not", 10, 0);
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
}

static token* consume_token()
{
	if (!head.next) return NULL;	
	struct token_stream_node* node = head.next;
	head.next = node->next;
	if (node->next) node->next->prev = &head;
	token* t = node->tkn; 
	free(node);

	return t;
}

static token* get_current_token()
{
	return (head.next) ? head.next->tkn : NULL;
}

static bool expect(token* t, token_type ttype)
{
	if (t)
	{
		return (t->type == ttype);
	}
	else
	{
		return false; 
	}
}

static void error_unexpected_token(token_type expected, token* current)
{
	if (current)
	{
		printf("Error: Expected %s, Parsed %s at line %d\n", TOKEN_TYPE_NAMES[expected], TOKEN_TYPE_NAMES[current->type], current->line_num);
		free(current);
	}
	else
	{
		printf("Error: Expected %s, Parsed NULL\n", TOKEN_TYPE_NAMES[expected]);
	}
	exit(1);
}

static void write_file(uint32_t instr)
{
	uint8_t p1 = instr >> 24;
	uint8_t p2 = instr >> 16;
	uint8_t p3 = instr >> 8;
	uint8_t p4 = instr;
	fwrite(&p2, 1, 1, output_file);
	fwrite(&p1, 1, 1, output_file);
	fwrite(&p4, 1, 1, output_file);
	fwrite(&p3, 1, 1, output_file);
}

static void write_all()
{
	for (unsigned int i = 0; i < instr_list_ptr; i++)
	{
		write_file(instr_list[i]->encoding);
	}
}

static void parse_reg(uint32_t* bin_instr, reg_type rtype)
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

	switch (rtype)
	{
		case REG_OP1:  *bin_instr |= reg->encoding << OP1_REG_SHIFT_VAL;
			       break;
		case REG_OP2:  *bin_instr |= reg->encoding << OP2_REG_SHIFT_VAL;
			       break;
		case REG_DEST: *bin_instr |= reg->encoding << DEST_REG_SHIFT_VAL;
			       break;
	}
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

static void parse_newline()
{
	token* t = consume_token();
	if (!expect(t, TOKEN_NEWLINE))
	{
		error_unexpected_token(TOKEN_NEWLINE, t);
	}
	free_token(t);	
	cur_addr = cur_addr + 4;
	while (get_current_token() && get_current_token()->type == TOKEN_NEWLINE)
	{
		consume_token();
	}
}

static void parse_num(uint32_t* bin_instr, int bits)
{
	uint32_t immediate_val;
	bool label_flag = false;
	token* t = consume_token();
	if (t->type == TOKEN_IDNT)
	{
		label* lbl = get_label(t->value);
		if (!lbl)
		{
			printf("ERROR: Referenced undefinied label\n");
			exit(1);
		}
		immediate_val = lbl->addr;
		label_flag = true;
	}
	else
	{

		if (!expect(t, TOKEN_DOLLAR))
		{
			error_unexpected_token(TOKEN_DOLLAR, t);
		}
		free_token(t);

		t = consume_token();
		if (!expect(t, TOKEN_NUM))
		{
			error_unexpected_token(TOKEN_NUM, t);
		}

		immediate_val = t->num_value;
	}
	switch (bits)
	{
		case IMM_22: immediate_val &= 0x003fffff;
			     *bin_instr |= immediate_val;
			     break;
		case IMM_17: immediate_val &= 0x00003fff;
			     *bin_instr |= immediate_val;
			     break;
		case OFFSET_19: if (label_flag) 
				{
					int dif = immediate_val - cur_addr;
					immediate_val = (dif < 0) ? immediate_val - cur_addr + 4 : immediate_val - cur_addr;
				}
			     	if (immediate_val >> 18) immediate_val |= 0xfff80000;
				else immediate_val &= 0x0007ffff;
				*bin_instr |= immediate_val & 0x0007ffff;
				break;
		case ADDR_18: immediate_val &= 0x0003ffff;
			      *bin_instr |= immediate_val;
			      break;
	}
	free_token(t);
}

static void gen_opcode4_instr(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	bin_instr |= knode->func;

	parse_reg(&bin_instr, REG_OP1);	

	parse_comma();

	parse_reg(&bin_instr, REG_OP2);
	
	parse_comma();

	parse_reg(&bin_instr, REG_DEST);

	parse_newline();

	add_instr_struct(bin_instr, false);

	sim_instr(bin_instr);

	//write_file(bin_instr);
}

static void gen_move_instr(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	
	parse_reg(&bin_instr, REG_OP1);
	
	parse_comma();

	parse_reg(&bin_instr, REG_DEST);
	
	parse_newline();	

	add_instr_struct(bin_instr, false);

	sim_instr(bin_instr);

	//write_file(bin_instr);
}

static void gen_loadi(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
		
	parse_num(&bin_instr, IMM_22);

	parse_comma();

	parse_reg(&bin_instr, REG_DEST);	

	parse_newline();	

	add_instr_struct(bin_instr, false);

	sim_instr(bin_instr);

	//write_file(bin_instr);

}

static void gen_addi(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;

	parse_num(&bin_instr, IMM_17);

	parse_comma();

	parse_reg(&bin_instr, REG_OP1);

	parse_comma();

	parse_reg(&bin_instr, REG_DEST);

	parse_newline();
	
	add_instr_struct(bin_instr, false);

	sim_instr(bin_instr);

	//write_file(bin_instr);
}

static void gen_not(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;

	parse_reg(&bin_instr, REG_OP1);
	
	parse_comma();

	parse_reg(&bin_instr, REG_DEST);

	parse_newline();

	add_instr_struct(bin_instr, false);

	sim_instr(bin_instr);

	//write_file(bin_instr);
}

static void gen_dynamic_instr(struct dyn_instr_struct* dyn_instr, bool jmp_tag)
{
	uint32_t bin_instr = 0;
	switch (dyn_instr->ditype)
	{
		case DYN_INSTR_ADD : bin_instr |= 4 << OPCODE_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_op1 << OP1_REG_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_op2 << OP2_REG_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_dest << DEST_REG_SHIFT_VAL;
				     break;
		case DYN_INSTR_ADDI: bin_instr |= 5 << OPCODE_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_op1 << OP1_REG_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_dest << DEST_REG_SHIFT_VAL;
				     bin_instr |= dyn_instr->num_val;
				     break;
		case DYN_INSTR_LOADI: bin_instr |= 3 << OPCODE_SHIFT_VAL;
				      bin_instr |= dyn_instr->reg_dest << DEST_REG_SHIFT_VAL;
				      bin_instr |= dyn_instr->num_val;
				      break;
		case DYN_INSTR_SHL: bin_instr |= 4 << OPCODE_SHIFT_VAL;
				    bin_instr |= 2;
				    bin_instr |= dyn_instr->reg_op1 << OP1_REG_SHIFT_VAL;
				    bin_instr |= dyn_instr->reg_op2 << OP2_REG_SHIFT_VAL;
				    bin_instr |= dyn_instr->reg_dest << DEST_REG_SHIFT_VAL;
				    break;
		case DYN_INSTR_NOT: bin_instr |= 10 << OPCODE_SHIFT_VAL; 
				    bin_instr |= dyn_instr->reg_op1 << OP1_REG_SHIFT_VAL;
				    bin_instr |= dyn_instr->reg_dest << DEST_REG_SHIFT_VAL;
				    break;
	}
	free(dyn_instr);
	dyn_instr_counter++;
	add_instr_struct(bin_instr, jmp_tag);
	update_addrs();
	cur_addr += 4;
	sim_instr(bin_instr);
	//write_file(bin_instr);
}

// r29,r28 reserved for jump addresses and scaling
static void gen_jump(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	uint32_t scale_reg = 0;
	bool scaling = false;
	
	// no scaling
	if ((get_current_token() && (get_current_token()->type == TOKEN_DOLLAR || get_current_token()->type == TOKEN_IDNT)))
	{
		parse_num(&bin_instr, ADDR_22);	
	}
	else
	{
		scaling = true;
		parse_reg(&bin_instr, REG_DEST);

		scale_reg = bin_instr >> DEST_REG_SHIFT_VAL;
		bin_instr = 0;

		parse_comma();

		parse_num(&bin_instr, ADDR_22);
	}
	uint32_t address = bin_instr;
	bin_instr = 0;
	
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, address), true);

	if (scaling) 
	{	
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, SCALE_REG, 22), false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, SCALE_REG, scale_reg, 0), false);
		gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0), false);
	}

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	bin_instr |= ADDR_REG << DEST_REG_SHIFT_VAL;

	parse_newline();

	add_instr_struct(bin_instr, false);

	//write_file(bin_instr);
}

static void gen_branches(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	bin_instr |= knode->func << BR_FUNC_SHIFT_VAL;

	parse_reg(&bin_instr, REG_DEST);

	parse_comma();

	parse_num(&bin_instr, OFFSET_19);

	parse_newline();

	add_instr_struct(bin_instr, true);

	//write_file(bin_instr);
}

static void gen_load(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;	
	
	parse_reg(&bin_instr, REG_OP1);
	uint32_t scale_reg = bin_instr >> OP1_REG_SHIFT_VAL;
	bin_instr = 0;

	parse_comma();

	parse_num(&bin_instr, ADDR_18);
	uint32_t addr = bin_instr;
	bin_instr = 0;

	parse_comma();

	parse_reg(&bin_instr, REG_DEST);
	uint32_t dest_reg = bin_instr >> DEST_REG_SHIFT_VAL;
	bin_instr = 0;

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, addr), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, SCALE_REG, 18), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, SCALE_REG, scale_reg, 0), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0), false);

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;	
	bin_instr |= dest_reg << DEST_REG_SHIFT_VAL;
	bin_instr |= ADDR_REG << OP1_REG_SHIFT_VAL;

	parse_newline();

	add_instr_struct(bin_instr, false);

	sim_instr(bin_instr);

	//write_file(bin_instr);
}

static void gen_store(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;	

	parse_reg(&bin_instr, REG_DEST);
	uint32_t src_reg = bin_instr >> DEST_REG_SHIFT_VAL;
	bin_instr = 0;

	parse_comma();

	parse_reg(&bin_instr, REG_OP1);
	uint32_t scale_reg = bin_instr >> OP1_REG_SHIFT_VAL;
	bin_instr = 0;

	parse_comma();

	parse_num(&bin_instr, ADDR_18);
	uint32_t addr = bin_instr;
	bin_instr = 0;

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, addr), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, SCALE_REG, 18), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, SCALE_REG, scale_reg, 0), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0), false);

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	bin_instr |= ADDR_REG << OP1_REG_SHIFT_VAL;
	bin_instr |= src_reg << DEST_REG_SHIFT_VAL;

	parse_newline();

	add_instr_struct(bin_instr, false);

	sim_instr(bin_instr);

	//write_file(bin_instr);
}

void gen_pseudo_sub()
{
	uint32_t bin_instr = 0;

	parse_reg(&bin_instr, REG_OP1);
	uint32_t reg_op1 = bin_instr >> OP1_REG_SHIFT_VAL;
	bin_instr = 0;

	parse_comma();

	parse_reg(&bin_instr, REG_OP2);
	uint32_t reg_op2 = bin_instr >> OP2_REG_SHIFT_VAL;
	bin_instr = 0;

	parse_comma();

	parse_reg(&bin_instr, REG_DEST);
	uint32_t reg_dest = bin_instr >> DEST_REG_SHIFT_VAL;
	
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_NOT, reg_op2, 0, reg_op2, 0), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADDI, reg_op2, 0, reg_op2, 1), false);
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, reg_op1, reg_op2, reg_dest, 0), false);

	parse_newline();
}

void gen_binary()
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
				parse_newline(current);	
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
				//write_file(bin_instr);
				add_instr_struct(bin_instr, false);
				token* t = consume_token();
				if (!expect(t, TOKEN_NEWLINE)) error_unexpected_token(TOKEN_NEWLINE, t);
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
			case 10: gen_not(knode);
				 break;
			case 32: gen_pseudo_sub();
				 break;
			default : exit(1);
		}		
	}
	write_all();
	free_reg_hashset();
	free_keyword_hashset();
}
