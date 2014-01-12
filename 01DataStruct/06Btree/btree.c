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


static void btree_pre_order(struct btree_info *info,
      void (*todo)(struct bnode_info *node))
{
	__pre_order(info->root, todo);
}

static void __in_order(struct bnode_info *cur, 
	void (*todo)(struct bnode_info *node))
{
	if (cur != NULL) {
		__in_order(cur->lchild, todo);
		todo(cur);
		__in_order(cur->rchild, todo);
	}
}


static void btree_in_order(struct btree_info *info,
      void (*todo)(struct bnode_info *node))
{
	__in_order(info->root, todo);
}

static void __post_order(struct bnode_info *cur, 
	void (*todo)(struct bnode_info *node))
{
	if (cur != NULL) {
		__post_order(cur->lchild, todo);
		__post_order(cur->rchild, todo);
		todo(cur);
	}
}




static void btree_post_order(struct btree_info *info,
      void (*todo)(struct bnode_info *node))
{
	__post_order(info->root, todo);
}

                                         

static void btree_level_order(struct btree_info *info,
                              void (*todo)(struct bnode_info *node))
{
	struct queue_info *queue = (struct queue_info *)
			malloc(sizeof(*queue));
	assert(queue != NULL);
	queue_init(queue);
	
	struct bnode_info *cur = info->root;
	queue->push(queue, &cur, sizeof(cur));

	while (!queue->is_empty(queue)) {
		queue->pop(queue, &cur, sizeof(cur));
		todo(cur);
		if (cur->lchild != NULL) {
			queue->push(queue, &cur->lchild, sizeof(cur));
		}
		if (cur->rchild != NULL) {
			queue->push(queue, &cur->rchild, sizeof(cur));
		}
	}

	queue_destroy(queue);
	free(queue);
}
static size_t __get_depth(const struct bnode_info *bnode)
{
	if (bnode == NULL) {
		return 0;
	} else {
		size_t ldepth = __get_depth(bnode->lchild);
		size_t rdepth = __get_depth(bnode->rchild);
		
		if (ldepth > rdepth) {
			return ldepth + 1;
		} else {
			return rdepth + 1;
		}
	}
}


static size_t btree_get_depth(const struct btree_info *info)
{
	assert(info != NULL);

	return __get_depth(info->root);
}


static struct bnode_info *btree_find(struct btree_info *info,
                                const struct bnode_info *key)
{
	assert(info != NULL);
	assert(key != NULL);
	
	struct bnode_info *cur = info->root;

	while (cur != NULL) {
		int n = info->cmp(key, cur);
		if (n == 0) {
			return cur;
		} else if (n < 0) {
			cur = cur->lchild;
		} else {
			cur = cur->rchild;
		}
	}
// record not found
	return NULL;
}

static int btree_del(struct btree_info *info, struct bnode_info *node)
{
	assert(info != NULL);
	assert(node);
	
	struct bnode_info **pparent = NULL;
	// btree is empty
	if (info->root == NULL) {
		return -1;	
	}

	if (node->parent == NULL) {
		pparent = &info->root;
	} else {
		if (node->parent->lchild == node) {
			pparent = &node->parent->lchild;
		} else {
			pparent = &node->parent->rchild;
		}
	}

	//1. 叶子
	if (node->lchild == NULL && node->rchild == NULL) {
		*pparent = NULL;
		node->parent = NULL;
	} else if (node->lchild != NULL && node->rchild == NULL) {	
		//2. 有左无右
		node->lchild->parent = node->parent;
		*pparent = node->lchild;
		node->parent = NULL;
		node->lchild = NULL; 
	} else if (node->rchild != NULL && node->lchild == NULL){
		//3. 有右无左
		node->rchild->parent = node->parent;
		*pparent = node->rchild;
		node->parent = NULL;
		node->rchild = NULL;
	} else {
		//4. 左右均有
		//4.1 找出合适的继承者(右子树中最小的节点)
		struct bnode_info *cur = node->rchild;
		while (cur->lchild != NULL) {
			cur = cur->lchild;
		}
		//4.2 从现有结构中将其删除
		if (cur->parent != node) { //不相邻
			cur->parent->lchild = cur->rchild;
		} else {
			//相邻
			cur->parent->rchild = cur->rchild;
		}
		
		if (cur->rchild != NULL) {
			cur->rchild->parent = cur->parent;
		}
		//4.3 替换被删除节点
		*cur = *node;
		//4.4 更新其他相邻节点 
		*pparent = cur;
		if (cur->lchild != NULL)
			cur->lchild->parent = cur;
		if (cur->rchild != NULL)
			cur->rchild->parent = cur;
	}

	return 0;
}


static void btree_pre_order_norecur(struct btree_info *info,
      void (*todo)(struct bnode_info *node))
{
	assert(info != NULL);
	assert(todo != NULL);

	struct bnode_info *cur = info->root;
	struct stack_info *stack = (struct stack_info *)
			malloc(sizeof(*stack));
	assert(stack != NULL);
	
	stack_init(stack);

	while (!stack->is_empty(stack) || cur != NULL) {
		if (cur != NULL) {
			stack->push(stack, &cur, sizeof(cur));
			todo(cur);
			cur = cur->lchild;
		} else {
			stack->pop(stack, &cur, sizeof(cur));
			cur = cur->rchild;
		}
	}

	stack_destroy(stack);
	free(stack);
}


static void btree_in_order_norecur(struct btree_info *info,
      void (*todo)(struct bnode_info *node))
{
	assert(info != NULL);
	assert(todo != NULL);

	struct bnode_info *cur = info->root;
	struct stack_info *stack = (struct stack_info *)
			malloc(sizeof(*stack));
	assert(stack != NULL);
	
	stack_init(stack);

	while (!stack->is_empty(stack) || cur != NULL) {
		if (cur != NULL) {
			stack->push(stack, &cur, sizeof(cur));
			cur = cur->lchild;
		} else {
			stack->pop(stack, &cur, sizeof(cur));
			todo(cur);
			cur = cur->rchild;
		}
	}

	stack_destroy(stack);
	free(stack);
}


static void btree_post_order_norecur(struct btree_info *info,
      void (*todo)(struct bnode_info *node))
{
	assert(info != NULL);
	assert(todo != NULL);

	struct bnode_info *cur = info->root;
	struct bnode_info *prev = NULL;
	struct stack_info *stack = (struct stack_info *)
			malloc(sizeof(*stack));
	assert(stack != NULL);
	
	stack_init(stack);

	while (!stack->is_empty(stack) || cur != NULL) {
		if (cur != NULL) {
			stack->push(stack, &cur, sizeof(cur));
			cur = cur->lchild;
		} else {
			stack->top(stack, &cur, sizeof(cur));
			if (cur->rchild != NULL && cur->rchild != prev) {
				cur = cur->rchild;
			} else {
				stack->pop(stack, &prev, sizeof(prev));
				todo(cur);
				cur = NULL;
			}
		}
	}

	stack_destroy(stack);
	free(stack);
}



int btree_init(struct btree_info *info,
	int (*cmp)(const struct bnode_info *, 			const struct bnode_info *))
{
	assert(info != NULL);
	assert(cmp != NULL);
	
	info->root = NULL;
	info->cmp = cmp;
	info->push = btree_push;
	info->pre_order = btree_pre_order;
	info->in_order = btree_in_order;
	info->post_order = btree_post_order;
	info->level_order = btree_level_order;
	info->get_depth = btree_get_depth;
	info->find = btree_find;
	info->del = btree_del;
	info->pre_order_norecur = btree_pre_order_norecur;
	info->in_order_norecur = btree_in_order_norecur;
	info->post_order_norecur = btree_post_order_norecur;
	return 0;
}
int btree_destroy(struct btree_info *info)
{
	return 0;
}

