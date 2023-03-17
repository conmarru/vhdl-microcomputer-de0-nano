#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include "../headers/defs.h"

static const int hex_digit_val[6] = { 10, 11, 12, 13, 14, 15 };

static bool is_hex_digit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

uint32_t strtohex(char* str)
{
	int val = 0;
	int k = 0;
	unsigned int i = 0;
	while (str[i] != 0x00) {
		if (!is_hex_digit(str[i])) {
			err_val = 1;
			return 0;
		}
		k = (str[i] >= 'a' && str[i] <= 'f') ? hex_digit_val[str[i] - 'a'] : str[i] - '0';
		val = val * 16 + k;
		i++;
	}

	return val;
}

uint32_t strtouint(char* str)
{
	int val = 0;
	int k = 0;
	unsigned int i = 0;
	while (str[i] != 0x00) {
		if (!isdigit(str[i])) {
			err_val = 1;
			return 0;
		}
		k = str[i] - '0';
		val = val * 10 + k;
		i++;
	}

	return val;
}

int32_t multiplex(int32_t inputA, int32_t inputB, int sig)
{
	return (sig == 0) ? inputA : inputB;
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
