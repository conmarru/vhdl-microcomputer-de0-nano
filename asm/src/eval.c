#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "../headers/scan.h"
#include "../headers/eval.h"

static struct ast_node* new_ast_node(token_type type, struct ast_node* left, struct ast_node* right, int32_t val, bool unary)
{
	struct ast_node* new = (struct ast_node*)malloc(sizeof(struct ast_node));

	new->type = type;
	new->left = left;
	new->right = right;
	new->value = val;
	new->unary = unary;

	return new;
}

static struct ast_node* parse_factor()
{
	struct ast_node* node = NULL;

	token* t = consume_token();

	if (check_ttype(t, TOKEN_NUM))
	{	
		node = new_ast_node(TOKEN_NUM, NULL, NULL, t->num_value, false);
	}
	else if (check_ttype(t, TOKEN_LPAREN))
	{
		node = parse_expr();
		t = consume_token();
		if (!check_ttype(t, TOKEN_RPAREN))
		{
			printf("ERROR: Syntax error at line %d\n", t->line_num);
			if (t) free(t);
			exit(1);
		}
	}
	else
	{
		printf("ERROR: Syntax error at line %d\n", t->line_num);
		if (t) free(t);
		exit(1);
	}
	
	free(t);
	return node;
}

static struct ast_node* parse_term()
{
	struct ast_node* left = NULL;
	struct ast_node* right = NULL;

	left = parse_factor();

	while ((get_current_token() && get_current_token()->type == TOKEN_STAR) || get_current_token()->type == TOKEN_SLASH)
	{
		token* t = consume_token();
		right = parse_factor();

		left = new_ast_node(t->type, left, right, 0, false);
		free(t);

		if (get_current_token() && get_current_token()->type == TOKEN_RPAREN)
		{
			return left;
		}
	}

	return left;
}

struct ast_node* parse_expr()
{
	struct ast_node* sign = NULL;
	struct ast_node* left = NULL;
	struct ast_node* right = NULL;	

	if (check_ttype(get_current_token(), TOKEN_PLUS) || check_ttype(get_current_token(), TOKEN_MINUS))
	{
		token* t = consume_token();
		sign = new_ast_node(t->type, NULL, NULL, 0, true);
	}

	left = parse_term();
	if (sign) sign->left = left;

	if (get_current_token() && get_current_token()->type == TOKEN_RPAREN)
	{
		return (sign) ? sign : left;
	}

	while (check_ttype(get_current_token(), TOKEN_PLUS) || check_ttype(get_current_token(), TOKEN_MINUS))
	{
		token* t = consume_token();

		right = parse_term();

		left = (sign) ? new_ast_node(t->type, sign, right, 0, false) : new_ast_node(t->type, left, right, 0, false);
		free(t);	

		if (get_current_token() && get_current_token()->type == TOKEN_RPAREN)
		{
			break;
		}
	}
	return left;
}

int32_t eval(struct ast_node* root)
{
	if (root)
	{
		switch (root->type)
		{
			case TOKEN_PLUS: return eval(root->left) + eval(root->right);
			case TOKEN_MINUS: return (root->unary) ? -eval(root->left) : eval(root->left) - eval(root->right);
			case TOKEN_STAR: return eval(root->left) * eval(root->right);
			case TOKEN_SLASH: return eval(root->left) / eval(root->right);
			default: return root->value;
		}
	}
	else
	{
		return 0;
	}
}

void dump_ast(struct ast_node* root)
{
	if (root)
	{
		printf("Token: %s\n", TOKEN_TYPE_NAMES[root->type]);
		printf("Child nodes: (\n");
		printf("\n");
		dump_ast(root->left);
		dump_ast(root->right);
		printf(")\n");
	}
}
