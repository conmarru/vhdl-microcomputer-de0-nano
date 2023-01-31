#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include "../headers/defs.h"
#include "../headers/cmds.h"
#include "../headers/utils.h"
#include "../headers/pipeline.h"
#include "../headers/seq.h"

#define MAX_FILENAME 200
#define SIZEBUF_LEN 10
#define DEFAULT_MEM_SIZE 2056

const uint32_t EXTRACT_MASK = 0x0000001f;
uint8_t flags = 0;

int err_val = 0;
unsigned int mem_size = 0;

static void print_usage_info()
{
	printf("usage: ./emu -i <filename> [flags]\n");
	printf("flags:\t-r, --regdump\n");
	printf("\t-m, --memdump\n");
	printf("\t-I, --interactive\n");
	printf("\t-n, --no-pipelining\n");
	printf("\t-s, --mem-size <amount>\n");
	printf("\t-H, --term-on-hlt\n");
	printf("\t-h, --help\n");
}

static void parse_cli_args(int argc, char** args, char* filename, char* mem_size)
{
	int c = 0; 
	opterr = 0;
	while (1)
	{
		static struct option long_options[] = {
			{"input", required_argument, NULL, 'i'},
			{"regdump", no_argument, NULL, 'r'},
			{"memdump", no_argument, NULL, 'm'},
			{"interactive", no_argument, NULL, 'I'},
			{"no-pipelining", no_argument, NULL, 'n'},
			{"mem-size", required_argument, NULL, 's'},
			{"term-on-hlt", no_argument, NULL, 'H'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
		};

		int option_index = 0;
		c = getopt_long(argc, args, "i:rmIns:Hh", long_options, &option_index);

		if (c == -1)
		{
			return;
		}

		switch (c)
		{
			case 'i' : memcpy(filename, optarg, MAX_FILENAME);
				   break;
			case 'r' : flags |= 1;
				   break;
			case 'm' : flags |= 2;
				   break;
			case 'I' : flags |= 4;
				   break;
			case 'n' : flags |= 8;
				   break;
			case 's' : memcpy(mem_size, optarg, SIZEBUF_LEN);
				   flags |= 16;
				   break;
			case 'H' : flags |= 32;
				   break;
			case 'h' : print_usage_info();
				   exit(0);
			case '?' : print_usage_info(); 
				   exit(1);
		}
	}
}

int main(int argc, char** args)
{
	char* filename = (char*)calloc(MAX_FILENAME+1, sizeof(char));
	char* mem_size_buf = (char*)calloc(SIZEBUF_LEN+1, sizeof(char));
	parse_cli_args(argc, args, filename, mem_size_buf);
	FILE* input_file = fopen(filename, "rb");
	mem_size = (flags & 0x10) ? strtouint(mem_size_buf) : DEFAULT_MEM_SIZE;
	if (err_val)
	{
		printf("ERROR: specified size must be a positive integer\n");
		exit(1);
	}
	free(filename);
	free(mem_size_buf);
	if (!input_file)
	{
		printf("ERROR: Failed to open input file\n");
		exit(1);
	}
	int ifd = fileno(input_file);
	int fsize = lseek(ifd, 0, SEEK_END);

	void* buf = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, ifd, 0);
	if (!buf)
	{
		printf("ERROR: Failed to open input file\n");
		exit(1);
	}

	uint8_t* binary_data = (uint8_t*)calloc(fsize,sizeof(uint8_t));
	memcpy(binary_data, buf, fsize);
	if (flags & 0x08) 
	{
		seq_run(binary_data, fsize, mem_size);
	}
	else
	{
		run(mem_size, binary_data, fsize);
	}	
	munmap(buf, fsize);
	fclose(input_file);
	free(binary_data);
	return 0;
}
