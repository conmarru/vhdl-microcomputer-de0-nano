#include <stdint.h>
#include <stdbool.h>
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
	while (str[i] != 0x00)
	{
		if (!is_hex_digit(str[i]))
		{
			err_val = 1;
			return 0;
		}
		k = (str[i] >= 'a' && str[i] <= 'f') ? hex_digit_val[str[i] - 'a'] : str[i] - '0';
		val = val * 16 + k;
		i++;
	}

	return val;
}
