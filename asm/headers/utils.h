#include <stdnoreturn.h>

unsigned int get_hash(char* str, unsigned int div);
noreturn void errn_exit(int errcode);
noreturn void err_exit(const char* restrict format, ...);
