#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "btree.h"

struct data_info {
	int data;
	struct bnode_info bnode;
};

int bnode_cmp(const struct bnode_info *a, const struct bnode_info *b)
{
	struct data_info *pa = list_entry(a, struct data_info, bnode);
	struct data_info *pb = list_entry(b, struct data_info, bnode);

	return pa->data - pb->data;
}

void print_bnode(struct bnode_info *node)
{
	struct data_info *pa = list_entry(node, struct data_info, bnode);

	printf("%d ", pa->data);
}


int main()
{
	struct data_info s[] = {
	{6}, {9}, {7}, {3}, {2}, {5}, {1}, {4}, {8},
	};

	struct btree_info *btree = (struct btree_info *)malloc(sizeof(*btree));
	
	assert(btree != NULL);

	btree_init(btree, bnode_cmp);

	int i;
	for (i = 0; i < sizeof s/ sizeof s[0]; ++i) {
	btree->push(btree, &s[i].bnode);
}
	printf("----btree_pre_order-----\n");
	btree->pre_order(btree, print_bnode);
	printf("\n");


	btree_destroy(btree);
	free(btree);
	return 0;

}
