#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <getopt.h>
#include "../headers/def.h"
#include "../headers/scan.h"
#include "../headers/parser.h"
#include "../headers/gen.h"
#include "../headers/instr.h"
#include "../headers/lbl.h"

#define MAX_FILENAME 100

FILE* input_file = NULL;
FILE* output_file = NULL;
uint8_t cli_flags = 0;
static bool input_flag = false;

static void print_usage()
{
	printf("usage: ./asm -i <input_file> [ flags ]\n");
	printf("flags:\t-o, --output <output_file>\n");
	printf("\t-n, --no-nops\n");
	printf("\t-b, --no-branch-adjustment\n");
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
			{"no-branch-adjustment", no_argument, NULL, 'b'},
			{"one-instr-line-format", no_argument, NULL, 'l'},
			{"show-debug-info", no_argument, NULL, 'd'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
		};

		int option_index = 0;
		c = getopt_long(argc, args, "i:o:nbdhl", long_options, &option_index);
		
		if (c == -1)
		{
			return;
		}

		switch (c)
		{
			case 'i': memcpy(filename_in, optarg, MAX_FILENAME);
				  input_flag = true;
				  break;
			case 'o': memcpy(filename_out, optarg, MAX_FILENAME);
				  break;
			case 'n': cli_flags |= NO_NOPS_FLAG;
				  break;
			case 'b': cli_flags |= NO_BR_ADJ_FLAG;
				  break;
			case 'd': cli_flags |= DEBUG_FLAG;
				  break;
			case 'l': cli_flags |= ONE_LINE_FFLAG;
				  break;
			case 'h': print_usage();
				  exit(0);
			case '?': print_usage();
				  exit(1);
		}
		
	}
}

int main(int argc, char** args)
{
	char* input_file_name = (char*)calloc(MAX_FILENAME+1, sizeof(char));
	char* output_file_name = (char*)calloc(MAX_FILENAME+1, sizeof(char));
	parse_args(argc, args, input_file_name, output_file_name);
	if (!input_flag)
	{
		print_usage();
		exit(1);
	}
	input_file = fopen(input_file_name, "r");
	if (output_file_name[0] != 0x00) 
	{
		output_file = (cli_flags & ONE_LINE_FFLAG) ? fopen(output_file_name, "w") : fopen(output_file_name, "wb");
	}	
	else 
	{
		output_file = (cli_flags & ONE_LINE_FFLAG) ? fopen("output", "w") : fopen("output", "wb");
	}
	free(input_file_name);
	free(output_file_name);
	if (!output_file)
	{
		printf("ERROR: Couldn't create output file\n");
		exit(1);
	}
	if (!input_file)
	{
		printf("ERROR: Couldn't open input file\n");
		exit(1);
	}
	int ifd = fileno(input_file);
	int fsize = lseek(ifd, 0, SEEK_END);

	void* buf = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, ifd, 0);
	if (buf == NULL) return 1; 
	char* input = (char*)calloc((fsize+1),sizeof(char));
	memcpy(input, buf, fsize);
	input[fsize] = 0x00;
	munmap(buf, fsize);
	fclose(input_file);
	init_scanner(input, fsize);
	parse();
	encode_instr();
	free(input);
	free_token_stream();
	free_instr_structs();
	free_reg_hashset();
	free_keyword_hashset();
	free_lbl_hashset();
	fclose(output_file);

	return 0;
}
