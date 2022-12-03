#include <stdio.h>
#include <stdlib.h>
#include "../headers/defs.h"

static const char* MNEMONICS[] = {"nop", "hlt", "move", "loadi", "add", "addi", "shr", "shl", "and", "or", "not", "jmp", "bre", "brl", "brg", "load", "store", "shrb", "shlb"};

void disasm_instr(uint32_t instr)
{
	uint8_t opcode = instr >> OPCODE_SHIFT_VAL;
	uint8_t dest_reg = (instr >> DEST_REG_SHIFT_VAL) & EXTRACT_MASK;
	uint8_t op1_reg = (instr >> OP1_REG_SHIFT_VAL) & EXTRACT_MASK;
	uint8_t op2_reg = (instr >> OP2_REG_SHIFT_VAL) & EXTRACT_MASK;
	uint8_t func = instr & EXTRACT_MASK;
	int32_t immediate;

	switch (opcode)
	{
		case 0: 
		case 1: printf(MNEMONICS[opcode]);
			break;
		case 2: printf("%s\t%%%s, %%%s", MNEMONICS[opcode], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
			break;
		case 3: immediate = instr & 0x003fffff;
			if (immediate >> 21) immediate |= 0xffc00000;
			printf("%s\t$%d, %%%s", MNEMONICS[opcode], immediate, REG_STRINGS[dest_reg]);
			break;
		case 4: switch (func)
			{
				case 0: printf("%s\t%%%s, %%%s, %%%s", MNEMONICS[opcode], REG_STRINGS[op1_reg], REG_STRINGS[op2_reg], REG_STRINGS[dest_reg]);
					break;
				case 1: printf("%s\t%%%s, %%%s, %%%s", MNEMONICS[8], REG_STRINGS[op1_reg], REG_STRINGS[op2_reg], REG_STRINGS[dest_reg]);
					break;
				case 2: printf("%s\t%%%s, %%%s, %%%s", MNEMONICS[9], REG_STRINGS[op1_reg], REG_STRINGS[op2_reg], REG_STRINGS[dest_reg]);
					break;
			}
			break;
		case 5: immediate = instr & 0x0001ffff;
			if (immediate >> 16) immediate |= 0xfffe0000;
			printf("%s\t%%%s, $%d, %%%s", MNEMONICS[opcode], REG_STRINGS[op1_reg], immediate, REG_STRINGS[dest_reg]);
			break;
		case 6: printf("%s\t%%%s", MNEMONICS[11], REG_STRINGS[dest_reg]);
			break;
		case 7: uint8_t func_3 = (instr >> 19) & 0x00000007;
			uint32_t offset = instr & 0x0007ffff;
			offset = (offset >> 18) ? offset | 0xfff80000 : offset;
			switch (func_3)
			{
				case 0: printf("%s\t%%%s, %d", MNEMONICS[12], REG_STRINGS[dest_reg], offset);
					break;
				case 1: printf("%s\t%%%s, %d", MNEMONICS[13], REG_STRINGS[dest_reg], offset);
					break;
				case 2: printf("%s\t%%%s, %d", MNEMONICS[14], REG_STRINGS[dest_reg], offset);
					break;
			}
			break;
		case 8: printf("%s\t%%%s, %%%s", MNEMONICS[15], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
			break;	
		case 9: printf("%s\t%%%s, %%%s", MNEMONICS[16], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
			break;
		case 10: printf("%s\t%%%s, %%%s", MNEMONICS[10], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
			 break;
		case 11: switch (func)
			 {
				 case 0: printf("%s\t%%%s, %%%s", MNEMONICS[6], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
					 break;
				 case 1: printf("%s\t%%%s, %%%s", MNEMONICS[7], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
					 break;
				 case 2: printf("%s\t%%%s, %%%s", MNEMONICS[17], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
					 break;
				 case 3: printf("%s\t%%%s, %%%s", MNEMONICS[18], REG_STRINGS[op1_reg], REG_STRINGS[dest_reg]);
					 break;
			 }
			 break;
		default: printf("ILLEGAL INSTRUCTION");
			 break;
	}
}
