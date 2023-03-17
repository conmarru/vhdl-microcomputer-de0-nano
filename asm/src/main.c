#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include "../headers/def.h"
#include "../headers/scan.h"
#include "../headers/parser.h"
#include "../headers/gen.h"
#include "../headers/instr.h"
#include "../headers/lbl.h"
#include "../headers/utils.h"

static FILE* input_file = NULL;
FILE* output_file = NULL;
uint8_t cli_flags = 0;
static bool input_flag = false;
long max_file_name;

static void print_usage(void)
{
	printf("usage: ./asm -i <input_file> [ flags ]\n");
	printf("flags:\t-o, --output <output_file>\n");
	printf("\t-n, --no-nops\n");
	printf("\t-l, --one-instr-line-format\n");
	printf("\t-d, --show-debug-info\n");
	printf("\t-h, --help\n");
}

void parse_args(int argc, char** args, char* filename_in, char* filename_out)
{
	int c = 0;
	opterr = 0;
	while (1)
	{
		static struct option long_options[] = {
			{"input", required_argument, NULL, 'i'},
			{"output", required_argument, NULL, 'o'},
			{"no-nops", no_argument, NULL, 'n'},
			{"one-instr-line-format", no_argument, NULL, 'l'},
			{"show-debug-info", no_argument, NULL, 'd'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
		};

		int option_index = 0;
		c = getopt_long(argc, args, "i:o:ndhl", long_options, &option_index);
		
		if (c == -1) {
			return;
		}

		switch (c) {
			case 'i': 
				memcpy(filename_in, optarg, max_file_name);
				input_flag = true;
				break;
			case 'o': 
				memcpy(filename_out, optarg, max_file_name);
				break;
			case 'n': 
				cli_flags |= NO_NOPS_FLAG;
				break;
			case 'd': 
				cli_flags |= DEBUG_FLAG;
				break;
			case 'l': 
				cli_flags |= ONE_LINE_FFLAG;
				break;
			case 'h': 
				print_usage();
				exit(EXIT_FAILURE);
			case '?': 
				print_usage();
				exit(EXIT_FAILURE);
		}

	}
}

int main(int argc, char** args)
{
	// get system limit for file path names
	max_file_name = pathconf("/", _PC_NAME_MAX);
	if (max_file_name == -1) {
		max_file_name = 100;
	}
	char* input_file_name = (char*)calloc(max_file_name+1, sizeof(char));
	if (!input_file_name) {
		errn_exit(errno);
	}

	char* output_file_name = (char*)calloc(max_file_name+1, sizeof(char));
	if (!output_file_name) {
		errn_exit(errno);
	}

	parse_args(argc, args, input_file_name, output_file_name);
	if (!input_flag) {
		print_usage();
		exit(EXIT_FAILURE);
	}

	input_file = fopen(input_file_name, "r");
	if (!input_file) {	
		errn_exit(errno);
	}
	if (output_file_name[0] != 0x00) {
		output_file = (cli_flags & ONE_LINE_FFLAG) ? fopen(output_file_name, "w") : fopen(output_file_name, "wb");
	}	
	else {
		output_file = (cli_flags & ONE_LINE_FFLAG) ? fopen("output", "w") : fopen("output", "wb");
	}
	free(input_file_name);
	free(output_file_name);
	if (!output_file) {	
		errn_exit(errno);	
	}
	
	int ifd = fileno(input_file);
	if (ifd == -1) {
		errn_exit(errno);		
	}
	off_t fsize = lseek(ifd, 0, SEEK_END);
	if (fsize == -1) {	
		errn_exit(errno);
	}

	// map input file into address space
	void* buf = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, ifd, 0);
	if ((intptr_t)buf == -1) {	
		errn_exit(errno);
	}
	char* input = (char*)calloc((fsize+1),sizeof(char));
	if (!input) {
		errn_exit(errno);
	}
	memcpy(input, buf, fsize);
	input[fsize] = 0x00;
	if (munmap(buf, fsize) == -1) {
		errn_exit(errno);
	}
	if (fclose(input_file) == EOF) {
		errn_exit(errno);
	}
	init_scanner(input, fsize);
	parse();
	encode_instr();
	free(input);
	free_token_stream();
	free_instr_structs();
	free_reg_hashset();
	free_keyword_hashset();
	free_lbl_hashset();
	if (fclose(output_file) == EOF) {
		errn_exit(errno);
	}

	return 0;
}
