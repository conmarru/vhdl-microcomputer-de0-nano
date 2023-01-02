extern const char* CMDS[];

char** parse_uinput(char* input, int* argc);
void reg_dump(int32_t* regfile, const char* reg_idnts[], int32_t pc_val);
void load_program(int32_t* mem, uint8_t* bin_data, int size, unsigned int memsize);
void print_mem(int32_t* mem, uint32_t start_addr, uint32_t end_addr, bool disasm_flag, bool verbose_flag, bool mflag, uint32_t maddr);
void free_userargs(char** args, int argc);
