#include <stdlib.h>

char** parse_uinput(char* input, int* argc)
{
	if (!input) return NULL;
	char** args = (char**)malloc(sizeof(char*)*1);	
	args[0] = NULL;

	unsigned int i = 0;	
	unsigned int j = 0;
	unsigned int counter = 0;
	char* arg = (char*)calloc(1, sizeof(char));
	arg[0] = 0x00;

	while (input[i] != 0x00)
	{
		if (input[i] == ' ')
		{
			arg = realloc(arg, ++j);
			arg[j-1] = 0x00;
			args[counter] = arg;
			args = realloc(args, ++counter);	
			args[counter] = NULL;
			arg = (char*)calloc(1, sizeof(char));
			j = 0;
			while (input[i] == ' ')
			{	
				i++;
			}
		}
		else
		{
			arg = realloc(arg, ++j);
			arg[j-1] = input[i++];	
		}	
	}

	arg = realloc(arg, ++j);
	arg[j-1] = 0x00;
	args[counter++] = arg;

	*argc = counter;
	return args;
}

void free_userargs(char** args, int argc)
{
	for (int i = 0; i < argc; i++)
	{
		free(args[i++]);		
	}
	free(args);
}
