struct instr_struct {
	bool jmp_tag;
	uint32_t instr_addr;
	uint8_t opcode;
	uint8_t dest;
	uint8_t op1;
	uint8_t op2;
	uint32_t immediate;
	uint8_t func;
};

void encode_instr();

