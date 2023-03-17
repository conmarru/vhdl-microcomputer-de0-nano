#include <stdint.h>

#ifndef LBL_HASHSET_SIZE
#define LBL_HASHSET_SIZE 20
#endif

typedef struct {
	char* name;
	uint32_t addr;	
} label;

struct label_node {
	struct label_node* next;
	label* lbl;
};

void init_label_hashset(void);
label* new_label(char* name, int line);
void add_label(label* lbl);
label* get_label(char* lbl);
void free_lbl_hashset(void);
