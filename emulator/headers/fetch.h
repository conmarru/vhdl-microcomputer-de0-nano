#include <stdint.h>

struct instr_mem_struct {
	int32_t addr_in;
	int32_t instr_out;
	int32_t* mem_array;
};

struct pc_struct {
	int used_jump_in;
	int used_branch_in;
	int hlt_in;
	int32_t next_pc_in;
	int32_t pc_out;
	int32_t next_pc_out;
	int32_t pc_temp;
	int32_t jmp_counter;
};

extern int32_t pc;
extern struct instr_mem_struct instr_mem;
extern struct pc_struct pc_str;
extern bool hlt_flag;

void fetch_update_pipeline_regs();
void update_pc(struct pc_struct* pc_strct);
void alloc_instr_mem(unsigned int mem);
void free_instr_mem();
void fetch();
