#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include "../headers/def.h"
#include "../headers/utils.h"
#include "../headers/parser.h"
#include "../headers/instr.h"

static void write_file(uint32_t instr)
{
	uint8_t p1 = instr >> 24;
	uint8_t p2 = instr >> 16;
	uint8_t p3 = instr >> 8;
	uint8_t p4 = instr;
	if (!(cli_flags & ONE_LINE_FFLAG)) {
		fwrite(&p2, 1, 1, output_file);
		fwrite(&p1, 1, 1, output_file);
		fwrite(&p4, 1, 1, output_file);
		fwrite(&p3, 1, 1, output_file);
	}
	else {
		fprintf(output_file, "%d\n", instr);	
	}
}

void encode_instr(void)
{
	for (unsigned int i = 0; i < instr_list_ptr; i++) {
		uint32_t encoding = instr_list[i]->opcode << OPCODE_SHIFT_VAL;
		switch (instr_list[i]->opcode) {
			case 0: 
			case 1: 
				break;
			case 2: 
				encoding |= instr_list[i]->dest << DEST_REG_SHIFT_VAL;
				encoding |= instr_list[i]->op1 << OP1_REG_SHIFT_VAL;
				break;
			case 3: 
				encoding |= instr_list[i]->dest << DEST_REG_SHIFT_VAL;
				encoding |= (0x0001ffff & instr_list[i]->immediate);
				break;
			case 4: 
				encoding |= instr_list[i]->dest << DEST_REG_SHIFT_VAL;
				encoding |= instr_list[i]->op1 << OP1_REG_SHIFT_VAL;
				encoding |= instr_list[i]->op2 << OP2_REG_SHIFT_VAL;
				encoding |= instr_list[i]->func;
				break;
			case 5: 
				encoding |= instr_list[i]->dest << DEST_REG_SHIFT_VAL;
				encoding |= instr_list[i]->op1 << OP1_REG_SHIFT_VAL;
				encoding |= (0x0001ffff & instr_list[i]->immediate);
				break;
			case 6: 
				encoding |= instr_list[i]->dest << DEST_REG_SHIFT_VAL;
				break;
			case 7: 
				encoding |= instr_list[i]->dest << DEST_REG_SHIFT_VAL;
				encoding |= instr_list[i]->immediate << 5;
				encoding |= instr_list[i]->func;
				break;
			case 8:	
				encoding |= instr_list[i]->dest << DEST_REG_SHIFT_VAL;
				encoding |= instr_list[i]->op1 << OP1_REG_SHIFT_VAL;
				break;
			case 9:	
				encoding |= instr_list[i]->dest << OP1_REG_SHIFT_VAL;
				encoding |= instr_list[i]->op1 << DEST_REG_SHIFT_VAL;
				break;
		}

		write_file(encoding);
	}
}
