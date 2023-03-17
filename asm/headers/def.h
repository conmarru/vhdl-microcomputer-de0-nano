#include <stdio.h>
#include <stdint.h>

#define NO_NOPS_FLAG 1
#define NO_BR_ADJ_FLAG 2
#define DEBUG_FLAG 4
#define ONE_LINE_FFLAG 8
#define REG_NUM 32
#define OPCODE_SHIFT_VAL 27
#define DEST_REG_SHIFT_VAL 22
#define OP1_REG_SHIFT_VAL 17
#define OP2_REG_SHIFT_VAL 12
#define BR_FUNC_SHIFT_VAL 19
#define IMM_17 17

#define OP_NOP 0
#define OP_HLT 1
#define OP_MOVE 2
#define OP_LOADI 3
#define OP_ADD 4
#define OP_SHR 4
#define OP_SHL 4
#define OP_SHRB 4
#define OP_SHLB 4
#define OP_AND 4
#define OP_OR 4
#define OP_NOT 4
#define OP_ADDI 5
#define OP_JMP 6
#define OP_BRE 7
#define OP_BRL 7
#define OP_BRG 7
#define OP_LOAD 8
#define OP_STORE 9
#define OP_SUB 32
#define OP_PUSH 33
#define OP_POP 34
#define OP_CALL 35
#define OP_RET 36
#define OP_MUL 37

#define FUNC_ADD 0
#define FUNC_SHR 1
#define FUNC_SHL 2
#define FUNC_AND 3
#define FUNC_OR 4
#define FUNC_BRE 4
#define FUNC_BRL 2
#define FUNC_BRG 1
#define FUNC_NOT 5
#define FUNC_SHRB 7
#define FUNC_SHLB 8

extern FILE* output_file;
extern uint8_t cli_flags;
extern const uint8_t ADDR_REG;
extern const uint8_t STACK_POINTER_REG;
