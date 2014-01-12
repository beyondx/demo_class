#pragma once
#include <stdio.h>

struct node_info {
	struct node_info *prev;
	struct node_info *next;
	char priv[0];
};

struct dlist_info {
	struct  node_info *head;
	void (*add)(struct dlist_info *info, const void *data, size_t data_size);
	void (*add_tail)(struct dlist_info *info, const void *data, size_t data_size);
	int (*is_empty)(const struct dlist_info *info);
	struct node_info *(*find)(struct dlist_info *info, 
			int (*match)(struct node_info *node));
	int (*del)(struct node_info *node);
	void (*for_each)(struct dlist_info *info, 
			void (*todo)(struct node_info *node));
};

int dlist_init(struct dlist_info *info);
int dlist_destroy(struct dlist_info *info);

