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
#include "../headers/instr.h"

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

void free_reg_hashset()
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

void free_keyword_hashset()
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
	add_keyword("bre", 7, 4);
	add_keyword("brl", 7, 2);
	add_keyword("brg", 7, 1);
	
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
		free_token(t);
		t = consume_token();
		if (!expect(t, TOKEN_RPAREN))
		{
			error_unexpected_token(TOKEN_RPAREN, t);
		}
		free_ast(root);
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
			free_token(t);
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
			immediate_val = (dif < 0) ? immediate_val - cur_addr + 1: immediate_val - cur_addr;
		}
	}
	free_token(t);

	immediate_val &= 0x0001ffff;
	return (int32_t)immediate_val;
}

static void gen_opcode4_instr(struct keyword_node* knode)
{
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

	add_instr_struct(false, knode->opcode, dest, op1, op2, 0, knode->func, false);

	parse_newline(false);
}

static void gen_move_instr(struct keyword_node* knode)
{
	uint8_t dest;
	uint8_t op1;

	op1 = parse_reg();

	parse_comma();

	dest = parse_reg();

	add_instr_struct(false, knode->opcode, dest, op1, 0, 0, 0, false);

	parse_newline(false);
}

static void gen_loadi(struct keyword_node* knode)
{
	int32_t immediate;
	uint8_t dest;

	immediate = parse_num(false);

	parse_comma();

	dest = parse_reg();

	add_instr_struct(false, knode->opcode, dest, 0, 0, immediate, 0, false);

	parse_newline(false);
}

static void gen_addi(struct keyword_node* knode)
{
	int32_t immediate;
	uint8_t dest;
	uint8_t op1;

	immediate = parse_num(false);

	parse_comma();

	op1 = parse_reg();

	parse_comma();

	dest = parse_reg();

	add_instr_struct(false, knode->opcode, dest, op1, 0, immediate, 0, false);

	parse_newline(false);

}

// uses r29 if destination specified as immediate
static void gen_jump(struct keyword_node* knode)
{
	uint32_t immediate = 0;
	uint32_t dest_reg = ADDR_REG;
	bool jmp_tag = true;

	token* t = get_current_token();
	if (check_ttype(t, TOKEN_PERCENT))
	{
		dest_reg = parse_reg();	
		jmp_tag = false;
	}
	else if (check_curttype(TOKEN_DOLLAR) || check_curttype(TOKEN_IDNT))
	{
		immediate = parse_num(false);	
	}	
	
	add_instr_struct(jmp_tag, knode->opcode, dest_reg, 0, 0, immediate, 0, jmp_tag);

	parse_newline(false);
}

static void gen_branches(struct keyword_node* knode)
{
	uint8_t dest;
	int32_t immediate;

	dest = parse_reg();

	parse_comma();

	immediate = parse_num(true);

	add_instr_struct(true, knode->opcode, dest, 0, 0, immediate, knode->func, false);

	parse_newline(false);

}

static void gen_load(struct keyword_node* knode)
{
	uint32_t immediate = 0;
	uint32_t dest_reg;
	uint32_t src_reg = ADDR_REG;
	bool imm = false;

	if (check_curttype(TOKEN_DOLLAR))
	{
		imm = true;
		immediate = parse_num(false);
	}
	else
	{
		src_reg = parse_reg();	 
	}

	parse_comma();

	dest_reg = parse_reg();

	add_instr_struct(false, knode->opcode, dest_reg, src_reg, 0, immediate, 0, imm);

	parse_newline(false);
}

static void gen_store(struct keyword_node* knode)
{
	bool imm = false;
	uint32_t src_reg = 0;
	uint32_t dest_reg = ADDR_REG;
	uint32_t immediate = 0;

	src_reg = parse_reg();

	parse_comma();
	
	if (check_curttype(TOKEN_DOLLAR))
	{
		imm = true;
		immediate = parse_num(false);
	}
	else
	{
		dest_reg = parse_reg();
	}

	add_instr_struct(false, knode->opcode, src_reg, dest_reg, 0, immediate, 0, imm);

	parse_newline(false);

}

static void gen_pseudo_sub()
{
	uint8_t reg_op1 = parse_reg();

	parse_comma();

	uint8_t reg_op2 = parse_reg();

	parse_comma();

	uint8_t reg_dest = parse_reg();

	add_instr_struct(false, 32, reg_dest, reg_op1, reg_op2, 0, 0, false);

	parse_newline(false);
}

static void gen_pseudo_push()
{
	uint8_t op1 = parse_reg();
	
	add_instr_struct(false, 33, 0, op1, 0, 0, 0, false);

	parse_newline(false);
}

static void gen_pseudo_pop()
{
	uint8_t op1 = parse_reg();
	
	add_instr_struct(false, 34, 0, op1, 0, 0, 0, false);

	parse_newline(false);
}

static void gen_pseudo_call()
{
	uint32_t addr = parse_num(false);
	
	add_instr_struct(true, 35, 0, 0, ADDR_REG, addr, 0, false);

	parse_newline(false);
}

static void gen_pseudo_ret()
{
	add_instr_struct(false, 36, 0, 0, 0, 0, 0, false);	

	parse_newline(false);
}

static void gen_pseudo_mul()
{	
	uint8_t reg_op1 = parse_reg();

	parse_comma();

	uint8_t reg_op2 = parse_reg();

	parse_comma();

	uint8_t reg_dest = parse_reg();
	
	add_instr_struct(false, 37, reg_dest, reg_op1, reg_op2, 0, 0, false);

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
				free(current);
				parse_newline(true);	
				continue;
			}
			else if (current->type == TOKEN_IDNT) printf("Invalid instruction found at line %d\n", current->line_num);
			else printf("ERROR: Instruction expected at line %d\n", current->line_num);
			exit(1);
		}
		free_token(current);
		switch (knode->opcode)
		{
			case 0: 
			case 1: 
				add_instr_struct(false, knode->opcode, 0, 0, 0, 0, 0, false);	
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
	
	resolve_pseudo_instr();

	if (!(cli_flags & NO_NOPS_FLAG))
	{
		gen_nops();
	}	
}
