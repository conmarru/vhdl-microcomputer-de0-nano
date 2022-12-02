#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "../headers/lbl.h"
#include "../headers/utils.h"

static struct label_node* label_hashset[LABEL_HASHSET_SIZE] = { NULL };

static struct label_node* new_label_node(struct label_node* next, label* lbl)
{
	struct label_node* new = (struct label_node*)malloc(sizeof(struct label_node));

	new->next = next;
	new->lbl = lbl;

	return new;
}

void init_label_hashset()
{
	for (int i = 0; i < LABEL_HASHSET_SIZE; i++)
	{
		label_hashset[i] = new_label_node(NULL, NULL);
	}
}

label* new_label(char* name, int line) 
{
	label* new = (label*)malloc(sizeof(label));

	new->name = name;
	new->addr = line * 4;

	return new;
}

void add_label(label* lbl)
{
	struct label_node* head = label_hashset[get_hash(lbl->name, LABEL_HASHSET_SIZE)];
	
	if (!head->next)
	{
		head->next = new_label_node(NULL, lbl); 
		return;
	}
	struct label_node* new = new_label_node(head->next, lbl);
	head->next = new;	
}

label* get_label(char* name)
{
	struct label_node* node = label_hashset[get_hash(name, LABEL_HASHSET_SIZE)];
	node = node->next;

	while (node)
	{
		if (!strcmp(name, node->lbl->name))
		{
			return node->lbl;
		}
		node = node->next;
	}

	return NULL;
}

void update_lbls(uint32_t cur_addr)
{
	struct label_node* node = NULL;
	for (int i = 0; i < LABEL_HASHSET_SIZE; i++)
	{
		node = label_hashset[i]->next;	
		while (node)
		{
			if (node->lbl->addr > cur_addr)
			{
				node->lbl->addr += 4;
				//printf("%s, %d\n", node->lbl->name, node->lbl->addr);
			}
			node = node->next;
		}
	}
}


