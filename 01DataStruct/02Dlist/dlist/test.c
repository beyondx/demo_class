#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dlist.h"

struct data_info {
	char *name;
	int age;
};

#define   N_ELEMT(a)	(sizeof(a) / sizeof(a[0]))

void print_node(struct  node_info *node)
{
	struct data_info *da = (struct data_info *)(node->priv);
	printf("%s:%d\n", da->name, da->age);
}

int find_lvbu(struct node_info *node)
{
	
	struct data_info *da = (struct data_info *)(node->priv);
	return !strcmp(da->name, "吕布");
}

int main()
{
	struct dlist_info *dlist = (struct dlist_info *)
		malloc(sizeof(*dlist));

	if (dlist == NULL) {
		perror("malloc");
		exit(1); 
	}

	dlist_init(dlist);

	struct data_info s[] = {
		{"张飞", 18}, 
		{"关羽", 20},
		{"刘备", 22},
		{"吕布", 19},
	};	

	int i;
	for (i = 0; i < N_ELEMT(s); ++i) {
		dlist->add_tail(dlist, s + i, sizeof(s[0]));
	}

	dlist->for_each(dlist, print_node);
	printf("\n");
	
	if (dlist->find(dlist, find_lvbu)) {
		printf("YES, Lv bu is here\n");
	} else {
		printf("No, lv bu is not found\n");
	}


	dlist_destroy(dlist);
	free(dlist);

	return 0;
}
