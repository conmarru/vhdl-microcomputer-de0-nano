#include <stdint.h>

#define OPCODE_SHIFT_VAL 27
#define DEST_REG_SHIFT_VAL 22
#define OP1_REG_SHIFT_VAL 17
#define OP2_REG_SHIFT_VAL 12
#define IMM_17_MASK 0x0001ffff
#define NOP_OPCODE 0
#define HLT_OPCODE 1
#define MOVE_OPCODE 2
#define LOADI_OPCODE 3
#define BIN_ALU_OP_OPCODE 4
#define ADDI_OPCODE 5
#define JMP_OPCODE 6
#define BRANCH_OPCODE 7
#define LOAD_OPCODE 8
#define STORE_OPCODE 9
#define NOT_OPCODE 10
#define SHIFT_OPCODE 11
#define REG_NUM 32
#define MAX_CMD_LEN 50
#define BRE_FUNC 4
#define BRL_FUNC 2
#define BRG_FUNC 1

extern const uint32_t EXTRACT_MASK;
extern const char* REG_STRINGS[];
extern int err_val;
extern uint8_t flags;
