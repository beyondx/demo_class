#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dlist.h"
/*有头循环双链表*/
#if 0
typedef  unsigned long size_t;
typedef  signed long ssize_t;
#endif

int dlist_is_empty(const struct dlist_info *info)
{
	return (info->head->next == info->head) && 
		(info->head->prev == info->head);
}

void dlist_add(struct dlist_info *info, const void *data, size_t data_size)
{
	assert(info != NULL);
	assert(data  != NULL);

	struct node_info *node = (struct node_info *)
		malloc(sizeof(struct node_info) + data_size);
	
	if (node == NULL) {
		perror("malloc");
		goto err_malloc;
	}

	node->next = info->head->next;
	node->prev = info->head;

	info->head->next = node;
	node->next->prev = node;

	memcpy(node->priv, data, data_size);

err_malloc:
	return ;
}

void dlist_add_tail(struct dlist_info *info, const void *data, size_t data_size)
{
	assert(info != NULL);
	assert(data  != NULL);

	struct node_info *node = (struct node_info *)
		malloc(sizeof(struct node_info) + data_size);
	
	if (node == NULL) {
		perror("malloc");
		goto err_malloc;
	}

	node->next = info->head;
	node->prev = info->head->prev;

	info->head->prev = node;
	node->prev->next = node;

	memcpy(node->priv, data, data_size);

err_malloc:
	return ;
}

void dlist_for_each(struct dlist_info *info, 
		void (*todo)(struct node_info *node))
{
	assert(info != NULL);
	assert(todo != NULL);
	
	struct node_info *cur = info->head->next;

	for (; cur != info->head; cur = cur->next) {
		todo(cur);
	}
}

void dlist_for_each_safe(struct dlist_info *info, 
		void (*todo)(struct node_info *node))
{
	assert(info != NULL);
	assert(todo != NULL);
	
	struct node_info *cur = info->head->next;
	struct node_info *Next = NULL;

	for (; cur != info->head; cur = Next) {
		Next = cur->next;
		todo(cur);
	}
}


void dlist_del(struct node_info *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;

	node->prev = node;
	node->next = node;

	free(node);
}

static struct node_info *dlist_find(struct dlist_info *info,
                        int (*match)(struct node_info *node))
{
	assert(info != NULL);
	assert(match != NULL);

	struct node_info *cur = info->head->next;
	for (; cur  != info->head; cur = cur->next) {
		if (match(cur)) {
			return cur;
		}
	}

	return NULL;
}


int dlist_init(struct dlist_info *info)
{
	assert(info != NULL);
	
	info->head = (struct node_info *)malloc(sizeof(struct node_info));
	
	if (info->head == NULL) {
		perror("malloc failed");
		goto err_malloc;
	}

	info->head->next = info->head;
	info->head->prev = info->head;

	info->is_empty = dlist_is_empty;
	info->add = dlist_add;
	info->add_tail = dlist_add_tail;
	info->del = dlist_del;
	info->find = dlist_find;
	info->for_each = dlist_for_each;
	info->for_each_safe = dlist_for_each_safe;

	return 0;
err_malloc:
	return -1;
}

int dlist_destroy(struct dlist_info *info)
{
	//TODO--- update xxx
	dlist_for_each_safe(info, dlist_del);
	free(info->head);
	return 0;
}
