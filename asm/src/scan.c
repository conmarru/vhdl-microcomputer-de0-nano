#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/mman.h>
#include "../headers/scan.h"

static char* input;
static struct token_stream_node head = {NULL, &head, NULL};
static int input_length = 0;
static int line = 1;

typedef enum {
	VAL_STR,
	VAL_NUM
} token_value_type;

char* TOKEN_TYPE_NAMES[] = {"Bezeichner", "Nummer", "Prozent", "Dollar", "Komma", "Newline"};

static token* new_token(char* value, int num_value, token_type type, token_value_type val_type)
{
	token* new = (token*)malloc(sizeof(token));
	if (val_type == VAL_STR) new->value = value; 
	else new->num_value = num_value;
	new->type = type;

	return new;
}

static struct token_stream_node* new_token_stream_node(struct token_stream_node* next, struct token_stream_node* prev, token* t)
{
	struct token_stream_node* new = (struct token_stream_node*)malloc(sizeof(struct token_stream_node));
	
	new->next = next;
	new->prev = prev;
	new->tkn = t;
	
	return new;
}

static void append_token(token* t)
{
	struct token_stream_node* node = &head;
	while (node->next != NULL)
	{
		node = node->next;
	}
	struct token_stream_node* new = new_token_stream_node(NULL, node, t);
	node->next = new;
}

static bool is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static char* scan_identifier(unsigned int* pointer)
{
	unsigned int idnt_length = 0;
	char* idnt = (char*)calloc(2, sizeof(char));
	idnt[1] = 0x00;

	while (isalpha(input[*pointer]) || is_digit(input[*pointer]))
	{
		idnt = realloc(idnt, ++idnt_length);
		idnt[idnt_length-1] = input[(*pointer)++];	
	}
	idnt = realloc(idnt, idnt_length+1);
	idnt[idnt_length] = 0x00;

	return idnt;
}

static int scan_num(unsigned int* pointer)
{
	int val = 0;
	int k = 0;
	while (is_digit(input[*pointer]))
	{
		k = input[(*pointer)++] - '0';
		val = val * 10 + k;
	}

	return val;
}

static void scan()
{		
	unsigned int pointer = 0;
	while (input[pointer] != 0x00 && pointer <= input_length)
	{
		switch (input[pointer])
		{
			case '%' : append_token(new_token(NULL, 0, TOKEN_PERCENT, VAL_NUM));
				   pointer++;
				   break;
			case '$' : append_token(new_token(NULL, 0, TOKEN_DOLLAR, VAL_NUM));
				   pointer++;
				   break;
			case ',' : append_token(new_token(NULL, 0, TOKEN_COMMA, VAL_NUM));
				   pointer++;
				   break;
			case '\n' : append_token(new_token(NULL, 0, TOKEN_NEWLINE, VAL_NUM));
				    pointer++;
				    line++;
				    break;
			case '\t' : 
			case ' ' : pointer++;
				   break;
			default : if (isalpha(input[pointer]))
				  {
				  	char* val = scan_identifier(&pointer);
					append_token(new_token(val, 0, TOKEN_IDNT, VAL_STR));
					break;
				  }
				  else if (is_digit(input[pointer]))
				  {
					int val = scan_num(&pointer);
					append_token(new_token(NULL, val, TOKEN_NUM, VAL_NUM));
					break;
				  }
				  else
				  {
					  printf("ERROR: Unknown token at line %d\n", line);
					  exit(1);
				  }
		}
	}
}

static void debug_print_token_stream()
{
	struct token_stream_node* node = head.next;
	while (node)
	{
		if (node->tkn->type == TOKEN_IDNT)
		{
			printf("Value: %s, Type: %s\n", node->tkn->value, TOKEN_TYPE_NAMES[node->tkn->type]);
		}
		else
		{
			printf("Value: %d, Type: %s\n", node->tkn->num_value, TOKEN_TYPE_NAMES[node->tkn->type]);
		}
		node = node->next;
	}
}

void init_scanner(char* buf, int length)
{
	input = (char*)calloc(length+1, sizeof(char));
	input_length = length;
	strcpy(input, buf);
	scan();
	debug_print_token_stream();
}
