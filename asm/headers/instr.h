struct instr_struct {
	bool jmp_tag;
	bool mltp_tag;
	uint32_t instr_addr;
	uint8_t opcode;
	uint8_t dest;
	uint8_t op1;
	uint8_t op2;
	uint32_t immediate;
	uint8_t func;
	struct instr_struct* jmp_target;
	struct instr_struct** p_from;
	unsigned int p_from_count;
};

typedef enum {
	DYN_INSTR_ADD,
	DYN_INSTR_ADDI,
	DYN_INSTR_LOADI,
	DYN_INSTR_SHL,
	DYN_INSTR_SHLB,
	DYN_INSTR_NOT,
	DYN_INSTR_LOAD,
	DYN_INSTR_STORE,
	DYN_INSTR_JMP,
	DYN_INSTR_BRE,
	DYN_INSTR_AND,
	DYN_INSTR_SHR,
	DYN_INSTR_MOVE,
	DYN_INSTR_NOP
} dyn_instr_type;

struct dyn_instr_struct {
	dyn_instr_type ditype;
	uint8_t reg_op1;
	uint8_t reg_op2;
	uint8_t reg_dest;
	int32_t num_val;
};

extern uint32_t cur_addr;
extern unsigned int instr_list_len;
extern unsigned int instr_list_ptr;
extern struct instr_struct** instr_list;

struct instr_struct* new_instr_struct(bool jmp_tag, uint32_t instr_addr, uint8_t opcode, 
						uint8_t dest, uint8_t op1, uint8_t op2, uint32_t immediate, uint8_t func, bool mltp_flag);
void free_instr_structs(void);
void add_instr_struct(bool jmp_flag, uint8_t opcode, uint8_t dest, uint8_t op1, uint8_t op2, 
						int32_t immediate, uint8_t func, bool mltp_flag);
struct dyn_instr_struct* new_dyn_instr_struct(dyn_instr_type ditype, uint8_t op1, uint8_t op2, 
								uint8_t dest, int32_t num);
void gen_nops(void);
void debug_print_instr_list(bool filter, int opcode, int func);
void resolve_pseudo_instr(void);
