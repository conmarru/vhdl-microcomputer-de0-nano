#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "../headers/lbl.h"
#include "../headers/utils.h"

// hashset that stores label definitions
static struct label_node* label_hashset[LBL_HASHSET_SIZE] = { NULL };

static struct label_node* new_label_node(struct label_node* next, label* lbl)
{
	struct label_node* new = (struct label_node*)malloc(sizeof(struct label_node));

	new->next = next;
	new->lbl = lbl;

	return new;
}

void init_label_hashset(void)
{
	for (int i = 0; i < LBL_HASHSET_SIZE; i++) {
		label_hashset[i] = new_label_node(NULL, NULL);
	}
}

label* new_label(char* name, int line) 
{
	label* new = (label*)malloc(sizeof(label));

	new->name = name;
	new->addr = line;

	return new;
}

void add_label(label* lbl)
{
	struct label_node* head = label_hashset[get_hash(lbl->name, LBL_HASHSET_SIZE)];
	
	if (!head->next) {
		head->next = new_label_node(NULL, lbl); 
		return;
	}
	struct label_node* new = new_label_node(head->next, lbl);
	head->next = new;	
}

label* get_label(char* name)
{
	struct label_node* node = label_hashset[get_hash(name, LBL_HASHSET_SIZE)];
	node = node->next;

	while (node) {
		if (!strcmp(name, node->lbl->name)) {
			return node->lbl;
		}
		node = node->next;
	}

	return NULL;
}

static void free_lbl(label* lbl)
{
	if (lbl->name) free(lbl->name);
	free(lbl);

}

static void free_lbl_node(struct label_node* node)
{
	if (node->lbl) free_lbl(node->lbl);
	free(node);
}

void free_lbl_hashset(void)
{
	for (int i = 0; i < LBL_HASHSET_SIZE; i++) {
		struct label_node* head = label_hashset[i];
		while (head->next) {
			struct label_node* node = head->next;
			head->next = node->next;
			free_lbl_node(node);
		}
		free_lbl_node(head);
	}
}
