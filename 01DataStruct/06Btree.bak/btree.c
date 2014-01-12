#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "btree.h"

static void btree_push(struct btree_info *info,
			struct bnode_info *node)
{
	assert(info != NULL);
	assert(node != NULL);
	
	bnode_init(node);

	// 1. 是否为空树
	if (info->root == NULL) {
		info->root = node;
		return ;
	}
	// 2.非空树
	struct bnode_info **pparent = &info->root;
	struct bnode_info *cur = NULL;

	while (*pparent != NULL) {
		cur = *pparent;
		if (info->cmp(node, cur) >= 0) {
			pparent = &cur->rchild;
		} else {
			pparent = &cur->lchild;
		}
	}  
	*pparent = node;
	node->parent = cur;

}
static void __pre_order(struct bnode_info *cur, 
	void (*todo)(struct bnode_info *node))
{
	if (cur != NULL) {
		todo(cur);
		__pre_order(cur->lchild, todo);
		__pre_order(cur->rchild, todo);
	}
}


void btree_pre_order(struct btree_info *info,
      void (*todo)(struct bnode_info *node))
{
	__pre_order(info->root, todo);
}
                                         

//void (*in_order)(struct btree_info *info,
        //                                                 void (*todo)(struct bnode_info *node));
        //                                                         void (*post_order)(struct btree_info *info,
        //                                                                          void (*todo)(struct bnode_info *node));
        //

int btree_init(struct btree_info *info,
	int (*cmp)(const struct bnode_info *, 			const struct bnode_info *))
{
	assert(info != NULL);
	assert(cmp != NULL);
	
	info->root = NULL;
	info->cmp = cmp;
	info->push = btree_push;
	info->pre_order = btree_pre_order;
	//info->in_order = btree_in_order;
	//info->post_order = btree_post_order;

	return 0;
}
int btree_destroy(struct btree_info *info)
{
	return 0;
}

