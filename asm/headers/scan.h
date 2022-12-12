#include <stdint.h>

#define LABEL_HASHSET_SIZE 20

typedef enum {
	TOKEN_IDNT,
	TOKEN_NUM,
	TOKEN_PERCENT,
	TOKEN_DOLLAR,
	TOKEN_COMMA,
	TOKEN_NEWLINE,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_PLUS,
	TOKEN_STAR,
	TOKEN_MINUS,
	TOKEN_SLASH
} token_type; 

typedef struct {
	union {
		char* value;	
		int num_value;
	};
	token_type type;
	int line_num;
} token;

struct token_stream_node {
	struct token_stream_node* next;
	struct token_stream_node* prev; 
	token* tkn;
};

extern char* TOKEN_TYPE_NAMES[];

extern struct token_stream_node head;

void init_scanner(char* buf, int length);
bool check_ttype(token* t, token_type ttype);
void free_token(token* t);
token* consume_token();
token* get_current_token();
token* peek();
bool expect(token* t, token_type ttype);
void error_unexpected_token(token_type expected, token* current);
void free_token_stream();
