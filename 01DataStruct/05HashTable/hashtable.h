#pragma once
#include "list.h"

/*KV keywords --> value  "tom" --> 1*/
typedef  size_t  val_t;

struct  hashtable {
	struct list_head *table;
	size_t nmemb;
	/*哈希函数*/
	val_t (*hash)(const struct list_head *node);
	/*哈希值的比较函数*/
	int (*hash_cmp)(const struct list_head *a, 
				const struct list_head *b);
	void (*push)(struct hashtable *info, struct list_head *node);
	void (*del)(struct hashtable *info, struct list_head *node);
	struct list_head *(*find)(const struct hashtable *info, 
					const struct list_head *key);
	//struct list_head *(*find)(const struct hashtable *info, 
	//		int (*match)(const struct list_head *));
	int (*is_empty)(const struct hashtable *info);
	int (*is_full)(const struct hashtable *info);	
	void (*for_each)(const struct hashtable *info, 
			void (*todo)(struct list_head *node));
};

int hashtable_init(struct hashtable *info, size_t nmemb, 
			val_t (*hash)(const struct list_head *node),
	int (*hash_cmp)(const struct list_head *a, 
				const struct list_head *b));
int hashtable_destroy(struct hashtable *info);

