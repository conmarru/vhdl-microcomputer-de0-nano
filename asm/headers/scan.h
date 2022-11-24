#include <stdint.h>

#define LABEL_HASHSET_SIZE 20

typedef enum {
	TOKEN_IDNT,
	TOKEN_NUM,
	TOKEN_PERCENT,
	TOKEN_DOLLAR,
	TOKEN_COMMA,
	TOKEN_NEWLINE
} token_type; 

typedef struct {
	union {
		char* value;	
		int num_value;
	};
	token_type type;
	int line_num;
} token;

typedef struct {
	char* name;
	uint32_t addr;	
} label;

struct token_stream_node {
	struct token_stream_node* next;
	struct token_stream_node* prev; 
	token* tkn;
};

struct label_node {
	struct label_node* next;
	label* lbl;
};

extern char* TOKEN_TYPE_NAMES[];

extern struct token_stream_node head;
extern struct label_node* label_hashset[LABEL_HASHSET_SIZE];

void init_scanner(char* buf, int length);
void free_token(token* t);
label* get_label(char* name);
