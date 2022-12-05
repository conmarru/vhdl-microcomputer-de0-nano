struct ast_node {
	token_type type;
	struct ast_node* left;
	struct ast_node* right;
	int32_t value;
	bool unary;
};

struct ast_node* parse_expr();
int32_t eval(struct ast_node* root);
void dump_ast(struct ast_node* root);
