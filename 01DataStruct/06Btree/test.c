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
	fflush(stdout);
}

void child_exchange(struct bnode_info *node)
{
	struct bnode_info *tmp = node->lchild;
	node->lchild = node->rchild;
	node->rchild = tmp;
}

int  find(struct  btree_info *info,   int key)
{
	struct data_info one = { .data = key };
	struct bnode_info *entry = NULL;
	entry = info->find(info, &one.bnode);
	
	if (entry == NULL) {
		fprintf(stderr, "key:%d not found\n", key);
		return -1;
	} else {
		struct data_info *pa = list_entry(entry, struct data_info, bnode);
		printf("found: %d\n", pa->data);
		return 0;
	}

}



int main()
{
	struct data_info s[] = {
	//{6}, {9}, {7}, {3}, {2}, {5}, {1}, {4}, {8},
	{6}, {10}, {3}, {2}, {5}, {1}, {4}, {9}, {11},
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
	printf("----btree_in_order-----\n");
	btree->in_order(btree, print_bnode);
	printf("\n");
	printf("----btree_post_order-----\n");
	btree->post_order(btree, print_bnode);
	printf("\n");
	printf("----btree_level_order-----\n");
	btree->level_order(btree, print_bnode);
	printf("\n");
	printf("depth:%lu\n", btree->get_depth(btree));
	#if 0
	btree->pre_order(btree, child_exchange);

	printf("--after exchage--btree_pre_order-----\n");
	btree->pre_order(btree, print_bnode);
	printf("\n");
	#endif
	
	//find(btree, 7);

	btree->del(btree, &s[1].bnode);
	printf("--after del--btree_pre_order-----\n");
	btree->pre_order(btree, print_bnode);
	printf("\n");
	printf("---btree_pre_order_norecur--\n");
	btree->pre_order_norecur(btree, print_bnode);
	printf("\n");
	printf("---btree_in_order_norecur--\n");
	btree->in_order_norecur(btree, print_bnode);
	printf("\n");
	printf("---btree_post_order_norecur--\n");
	btree->post_order_norecur(btree, print_bnode);
	printf("\n");

	btree_destroy(btree);
	free(btree);
	return 0;

}
