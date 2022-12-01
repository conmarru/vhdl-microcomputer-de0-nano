#include <stdlib.h>
#include <stdint.h>

static int32_t regs[32] = { 0 };

void write_reg(uint8_t reg_num, int32_t val)
{
	if (reg_num >= 32)
	{
		return;
	}	
	regs[reg_num] = val;
}

int32_t read_reg(uint8_t reg_num)
{
	return (reg_num < 32) ? regs[reg_num] : 0;
}

void sim_instr(uint32_t instr)
{
			
}
