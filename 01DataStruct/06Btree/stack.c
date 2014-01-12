#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stack.h"

static int stack_is_empty(struct stack_info *info)
{
	return info->dlist->is_empty(info->dlist);
}

static int stack_push(struct stack_info *info, 
			const void *data, size_t size)
{
	info->dlist->add(info->dlist, data, size);
	return 0;
}

static int stack_top(struct stack_info *info, 
				void *dest, size_t size)
{
	if (stack_is_empty(info)) {
		return -1;
	}
	
	memcpy(dest, info->dlist->head->next->priv, size);

	return 0;
}

static int stack_pop(struct stack_info *info, 
				void *dest, size_t size)
{
	if (stack_top(info, dest, size) < 0) {
		return -1;
	}
	
	info->dlist->del(info->dlist->head->next);

	return 0;
}

int stack_init(struct stack_info *info)
{
	info->dlist = (struct dlist_info *)malloc(sizeof(struct dlist_info));
	if (info->dlist == NULL) {
		perror("malloc");
		goto err_malloc;
	}

	dlist_init(info->dlist);
	
	info->push = stack_push;
	info->top = stack_top;
	info->pop = stack_pop;
	info->is_empty = stack_is_empty;

	return 0;

err_malloc:
	return -1;
}


int stack_destroy(struct stack_info *info)
{
	dlist_destroy(info->dlist);
	free(info->dlist);
	return 0;
}
