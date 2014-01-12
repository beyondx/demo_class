#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"

static int queue_is_empty(struct queue_info *info)
{
	return info->dlist->is_empty(info->dlist);
}

static int queue_push(struct queue_info *info, 
			const void *data, size_t size)
{
	info->dlist->add_tail(info->dlist, data, size);
	return 0;
}

static int queue_top(struct queue_info *info, 
				void *dest, size_t size)
{
	if (queue_is_empty(info)) {
		return -1;
	}
	
	memcpy(dest, info->dlist->head->next->priv, size);

	return 0;
}

static int queue_pop(struct queue_info *info, 
				void *dest, size_t size)
{
	if (queue_top(info, dest, size) < 0) {
		return -1;
	}
	
	info->dlist->del(info->dlist->head->next);

	return 0;
}

int queue_init(struct queue_info *info)
{
	info->dlist = (struct dlist_info *)malloc(sizeof(struct dlist_info));
	if (info->dlist == NULL) {
		perror("malloc");
		goto err_malloc;
	}

	dlist_init(info->dlist);
	
	info->push = queue_push;
	info->top = queue_top;
	info->pop = queue_pop;
	info->is_empty = queue_is_empty;

	return 0;

err_malloc:
	return -1;
}


int queue_destroy(struct queue_info *info)
{
	dlist_destroy(info->dlist);
	free(info->dlist);
	return 0;
}
