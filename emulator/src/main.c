#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include "../headers/defs.h"
#include "../headers/cmds.h"
#include "../headers/utils.h"
#include "../headers/pipeline.h"
#include "../headers/seq.h"

#ifndef SIZEBUF_LEN
#define SIZEBUF_LEN 10
#endif

#ifndef DEFAULT_MEM_SIZE
#define DEFAULT_MEM_SIZE 2056
#endif

static long max_file_name;
const uint32_t EXTRACT_MASK = 0x0000001f;
uint8_t flags = 0;

int err_val = 0;
unsigned int mem_size = 0;

static void print_usage_info(char* progname)
{
	printf("usage: %s -i <filename> [flags]\n", progname);
	printf("flags:\t-r, --regdump\n");
	printf("\t-m, --memdump\n");
	printf("\t-I, --interactive\n");
	printf("\t-n, --no-pipelining\n");
	printf("\t-s, --mem-size <amount>\n");
	printf("\t-H, --term-on-hlt\n");
	printf("\t-h, --help\n");
}

static void parse_cli_argv(int argc, char** argv, char* filename, char* mem_size)
{
	int c = 0; 
	opterr = 0;
	while (1) {
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
		c = getopt_long(argc, argv, "i:rmIns:Hh", long_options, &option_index);

		if (c == -1) {
			return;
		}

		switch (c) {
			case 'i': 
				memcpy(filename, optarg, max_file_name);
				break;

			case 'r': 
				flags |= REGDUMP_FLAG;
				break;

			case 'm': 
				flags |= MEMDUMP_FLAG;
				break;

			case 'I': 
				flags |= INTRCTV_FLAG;
				break;

			case 'n': 
				flags |= NO_PIPEL_FLAG;
				break;

			case 's': 
				memcpy(mem_size, optarg, SIZEBUF_LEN);
				flags |= MEMSZ_FLAG;
				break;

			case 'H': 
				flags |= TERMHLT_FLAG;
				break;

			case 'h': 
				print_usage_info(argv[0]);
				exit(EXIT_FAILURE);

			case '?': 
				print_usage_info(argv[0]); 
				exit(EXIT_FAILURE);

		}
	}
}

int main(int argc, char** argv)
{
	max_file_name = pathconf("/", _PC_NAME_MAX);	
	char* filename = (char*)calloc(max_file_name+1, sizeof(char));
	if (!filename) {
		errn_exit(errno);
	}
	char* mem_size_buf = (char*)calloc(SIZEBUF_LEN+1, sizeof(char));
	if (!mem_size_buf) {
		errn_exit(errno);
	}
	parse_cli_argv(argc, argv, filename, mem_size_buf);
	FILE* input_file = fopen(filename, "rb");
	if (!input_file) {
		errn_exit(errno);
	}
	mem_size = (flags & 0x10) ? strtouint(mem_size_buf) : DEFAULT_MEM_SIZE;
	if (err_val) {
		err_exit("specified size must be a positive integer\n");
	}
	free(filename);
	free(mem_size_buf);	
	int ifd = fileno(input_file);
	if (ifd == -1) {
		errn_exit(errno);
	}
	off_t fsize = lseek(ifd, 0, SEEK_END);
	if (fsize == -1) {
		errn_exit(errno);
	}

	void* buf = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, ifd, 0);
	if ((intptr_t)buf == -1)
	{
		errn_exit(errno);
	}

	uint8_t* binary_data = (uint8_t*)calloc(fsize,sizeof(uint8_t));
	if (!binary_data) {
		errn_exit(errno);
	}
	memcpy(binary_data, buf, fsize);
	if (flags & NO_PIPEL_FLAG) {
		seq_run(binary_data, fsize, mem_size);
	} else {
		run(mem_size, binary_data, fsize);
	}	
	if (munmap(buf, fsize) == -1) {
		errn_exit(errno);
	}
	if (fclose(input_file) == -1) {
		errn_exit(errno);
	}
	free(binary_data);
	return 0;
}
