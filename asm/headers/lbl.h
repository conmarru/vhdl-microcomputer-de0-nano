#include <stdint.h>

#define LABEL_HASHSET_SIZE 20

typedef struct {
	char* name;
	uint32_t addr;	
} label;

struct label_node {
	struct label_node* next;
	label* lbl;
};

void init_label_hashset();
label* new_label(char* name, int line);
void add_label(label* lbl);
label* get_label();
void update_lbls(uint32_t cur_addr);
void free_lbl_hashset();
