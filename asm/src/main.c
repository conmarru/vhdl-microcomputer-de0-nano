#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include "../headers/def.h"
#include "../headers/scan.h"

FILE* input_file = NULL;

int main(int argc, char** args)
{
	if (argc != 2)
	{
		return 1;
	}
	input_file = fopen(args[1], "r");
	if (input_file == NULL) return 1;
	int ifd = fileno(input_file);
	int fsize = lseek(ifd, 0, SEEK_END);

	void* buf = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, ifd, 0);
	if (buf == NULL) return 1; 
	char* input = (char*)calloc((fsize+1),sizeof(char));
	memcpy(input, buf, fsize);
	input[fsize] = 0x00;
	munmap(buf, fsize);
	init_scanner(input, fsize);
	free(input);

	return 0;
}
