#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "../headers/scan.h"

static char* input;
static struct token_stream_node head = {NULL, &head, NULL};
static int input_length = 0;

char* TOKEN_TYPE_NAMES[] = {"Register", "Instruktion", "Nummer", "Prozent", "Komma"};

static token* new_token(char* value, token_type type)
{
	token* new = (token*)malloc(sizeof(token));
	new->value = value; 
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

static bool isIdntChar(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

static void scan()
{		
	unsigned int pointer = 0;
	while (input[pointer] != 0x00 && pointer <= input_length)
	{
		// TODO: scan input
	}
}

static void debug_print_token_stream()
{
	struct token_stream_node* node = head.next;
	while (node)
	{
		printf("Value: %s, Type: %s\n", node->tkn->value, TOKEN_TYPE_NAMES[node->tkn->type]);
		node = node->next;
	}
}

void init_scanner(char* buf, int length)
{
	input = (char*)calloc(length+1, sizeof(char));
	input_length = length;
	strcpy(input, buf);
	scan();
}
