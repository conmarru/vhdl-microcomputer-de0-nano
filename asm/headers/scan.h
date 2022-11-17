typedef enum {
	TOKEN_REG,
	TOKEN_INSTR,
	TOKEN_NUM,
	TOKEN_PERCENT,
	TOKEN_COMMA,
	TOKEN_NEWLINE
} token_type; 

typedef struct {
	char* value;	
	token_type type;
} token;

struct token_stream_node {
	struct token_stream_node* next;
	struct token_stream_node* prev; 
	token* tkn;
};

void init_scanner(char* buf, int length);
