#pragma once
#include "dlist.h"

struct queue_info {
	struct dlist_info *dlist;
	int (*push)(struct queue_info *info, 
			const void *data, size_t size);
	int (*top)(struct queue_info *info, 
			void *dest, size_t size);
	int (*pop)(struct queue_info *info, 
			void *dest, size_t size);
	int (*is_empty)(struct queue_info *info);
};

int queue_init(struct queue_info *queue);
int queue_destroy(struct queue_info *queue);
