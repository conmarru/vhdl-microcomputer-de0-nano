#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include "../headers/scan.h"
#include "../headers/def.h"

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

typedef enum {
	REG_OP1,
	REG_OP2,
	REG_DEST
} reg_type;

typedef enum {
	DYN_INSTR_ADD,
	DYN_INSTR_ADDI,
	DYN_INSTR_LOADI,
	DYN_INSTR_SHL
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

static struct keyword_node* keyword_hashset[KEYWORD_HASHSET_LEN] = { NULL };
static struct reg_struct_node* reg_hashset[REG_NUM] = { NULL };

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

unsigned int get_reg_hash(char* name)
{
	unsigned int i = 0; 
	unsigned int hash = 0; 

	while (name[i] != 0x00)
	{
		hash += name[i] + i;
		i++;
	}

	return hash % REG_NUM;
}

void add_reg(char* name, uint8_t encoding)
{
	struct reg_struct_node* head = reg_hashset[get_reg_hash(name)];	
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
	struct reg_struct_node* node = reg_hashset[get_reg_hash(name)]->next;

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

static unsigned int get_keyword_hash(char* keyword)
{
	unsigned int i = 0;
	unsigned int hash = 0;

	while (keyword[i] != 0x00)
	{
		hash += keyword[i] + i;
		i++;
	}

	return hash % KEYWORD_HASHSET_LEN;
}

static void add_keyword(char* keyword, uint8_t opcode, uint8_t func)
{
	struct keyword_node* head = keyword_hashset[get_keyword_hash(keyword)];

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
	struct keyword_node* node = keyword_hashset[get_keyword_hash(keyword)]->next;

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
}

static void parse_num(uint32_t* bin_instr, int bits)
{
	token* t = consume_token();
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

	uint32_t immediate_val = t->num_value;
	switch (bits)
	{
		case IMM_22: immediate_val &= 0x003fffff;
			     *bin_instr |= immediate_val;
			     break;
		case IMM_17: immediate_val &= 0x00003fff;
			     *bin_instr |= immediate_val;
			     break;
		case OFFSET_19: immediate_val &= 0x0007ffff;
				*bin_instr |= immediate_val;
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

	write_file(bin_instr);
}

static void gen_move_instr(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	
	parse_reg(&bin_instr, REG_OP1);
	
	parse_comma();

	parse_reg(&bin_instr, REG_DEST);
	
	parse_newline();	

	write_file(bin_instr);
}

static void gen_loadi(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
		
	parse_num(&bin_instr, IMM_22);

	parse_comma();

	parse_reg(&bin_instr, REG_DEST);	

	parse_newline();	

	write_file(bin_instr);

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
	
	write_file(bin_instr);
}

static void gen_not(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;

	parse_reg(&bin_instr, REG_OP1);
	
	parse_comma();

	parse_reg(&bin_instr, REG_DEST);

	parse_newline();

	write_file(bin_instr);
}

static void gen_dynamic_instr(struct dyn_instr_struct* dyn_instr)
{
	uint32_t bin_instr = 0;
	switch (dyn_instr->ditype)
	{
		case DYN_INSTR_ADD : bin_instr |= 4 << OPCODE_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_op1 << OP1_REG_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_op2 << OP2_REG_SHIFT_VAL;
				     bin_instr |= dyn_instr->reg_dest << DEST_REG_SHIFT_VAL;
				     break;
		case DYN_INSTR_ADDI: break;
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
	}
	free(dyn_instr);
	write_file(bin_instr);
}

// r29,r28 reserved for jump addresses and scaling
static void gen_jump(struct keyword_node* knode)
{
	uint32_t bin_instr = 0;
	parse_reg(&bin_instr, REG_DEST);
	
	uint8_t scale_reg = bin_instr >> DEST_REG_SHIFT_VAL;
	bin_instr = 0;

	parse_comma();

	parse_num(&bin_instr, ADDR_22);

	uint32_t address = bin_instr;
	bin_instr = 0;
	
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, address));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, SCALE_REG, 22));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, SCALE_REG, scale_reg, 0));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0));
	
	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	bin_instr |= ADDR_REG << DEST_REG_SHIFT_VAL;

	parse_newline();

	write_file(bin_instr);
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

	write_file(bin_instr);
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

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, addr));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, SCALE_REG, 18));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, SCALE_REG, scale_reg, 0));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0));

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;	
	bin_instr |= dest_reg << DEST_REG_SHIFT_VAL;
	bin_instr |= ADDR_REG << OP1_REG_SHIFT_VAL;

	parse_newline();

	write_file(bin_instr);
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

	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, ADDR_REG, addr));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_LOADI, 0, 0, SCALE_REG, 18));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_SHL, scale_reg, SCALE_REG, scale_reg, 0));
	gen_dynamic_instr(new_dyn_instr_struct(DYN_INSTR_ADD, scale_reg, ADDR_REG, ADDR_REG, 0));

	bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
	bin_instr |= ADDR_REG << OP1_REG_SHIFT_VAL;
	bin_instr |= src_reg << DEST_REG_SHIFT_VAL;

	parse_newline();

	write_file(bin_instr);
}

void gen_binary()
{
	init_keyword_hashset();
	init_reg_hashset();
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
			if (current->type == TOKEN_IDNT) printf("Invalid instruction found at line %d\n", current->line_num);
			else printf("ERROR: Instruction expected at line %d\n", current->line_num);
			exit(1);
		}
		switch (knode->opcode)
		{
			case 0: 
			case 1: uint32_t bin_instr = 0; 
				bin_instr |= knode->opcode << OPCODE_SHIFT_VAL;
				write_file(bin_instr);
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
			default : exit(1);
		}		
	}

	free_reg_hashset();
	free_keyword_hashset();
}
