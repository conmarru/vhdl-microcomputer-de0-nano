#include <stdnoreturn.h>

uint32_t strtohex(char* str);
uint32_t strtouint(char* str);
int32_t multiplex(int32_t inputA, int32_t inputB, int sig);
noreturn void errn_exit(int errcode);
noreturn void err_exit(const char* restrict format, ...);
