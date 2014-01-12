#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
struct  node_info {
	int data;
	struct node_info *next;
};

#define N_ELEMT(a)	(sizeof(a) / sizeof(a[0]))

void slist_add(struct node_info **pphead, struct node_info *node)
{
	assert(pphead != NULL);
	 	//空链表 
	if (*pphead == NULL) {
		node->next = NULL;
	} else {
		//非空链表
		node->next = *pphead;
	}
	
	*pphead = node;	
}

void slist_for_each(struct node_info *phead) 
{
	struct node_info *cur = phead;

	for (;cur != NULL; cur = cur->next) {
		printf("%d ", cur->data);
	}
}

int main()
{
	struct node_info *phead = NULL;
	
	struct node_info s[] = {
		{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8},
	};

	int i;
	for (i = 0; i < N_ELEMT(s); ++i) {
		slist_add( &phead, s + i);
	}


	slist_for_each(phead);


	return 0;
}
