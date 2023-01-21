#include <stdio.h>
#include <stdint.h>

#define NO_NOPS_FLAG 1
#define KEYWORD_HASHSET_LEN 20
#define REG_NUM 32
#define OPCODE_SHIFT_VAL 27
#define DEST_REG_SHIFT_VAL 22
#define OP1_REG_SHIFT_VAL 17
#define OP2_REG_SHIFT_VAL 12
#define BR_FUNC_SHIFT_VAL 19
#define IMM_17 17

extern FILE* input_file;
extern FILE* output_file;
extern uint8_t cli_flags;
