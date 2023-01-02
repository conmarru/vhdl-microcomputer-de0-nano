#include <stdint.h>

struct instr_mem_struct {
	int32_t addr_in;
	int32_t instr_out;
	int32_t* mem_array;
};

extern int32_t pc;
extern struct instr_mem_struct instr_mem;

void fetch_update_pipeline_regs();
void update_pc(int32_t new_pc);
void alloc_instr_mem(unsigned int mem);
void free_instr_mem();
void fetch();
