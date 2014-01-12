#pragma once
#include "dlist.h"

struct stack_info {
	struct dlist_info *dlist;
	int (*push)(struct stack_info *info, 
			const void *data, size_t size);
	int (*top)(struct stack_info *info, 
			void *dest, size_t size);
	int (*pop)(struct stack_info *info, 
			void *dest, size_t size);
	int (*is_empty)(struct stack_info *info);
};

int stack_init(struct stack_info *stack);
int stack_destroy(struct stack_info *stack);
