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

struct token_stream_node {
	struct token_stream_node* next;
	struct token_stream_node* prev; 
	token* tkn;
};

extern char* TOKEN_TYPE_NAMES[];

extern struct token_stream_node head;

void init_scanner(char* buf, int length);
void free_token(token* t);
