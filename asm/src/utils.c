#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>

unsigned int get_hash(char* str, unsigned int div)
{
	unsigned int i = 0; 
	unsigned int hash = 0; 

	while (str[i] != 0x00) {
		hash += str[i] + i;
		i++;
	}
	
	return hash % div;
}

noreturn void errn_exit(int errcode) 
{
	fprintf(stderr, "ERROR: %s\n", strerror(errcode));
	exit(EXIT_FAILURE);
}

noreturn void err_exit(const char* restrict format, ...) 
{
	va_list arg_list;
	va_start(arg_list, format);
	fprintf(stderr, "ERROR: ");
	vfprintf(stderr, format, arg_list);
	va_end(arg_list);
	exit(EXIT_FAILURE);
}
