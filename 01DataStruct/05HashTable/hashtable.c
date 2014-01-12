#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashtable.h"


struct list_head *hash_find(const struct hashtable *info,
                        const struct list_head *key)
{
	assert(info != NULL);
	assert(key != NULL);

	val_t v = info->hash(key) % info->nmemb;
#if 0	
	struct  list_head *cur = info->table[v].next;

	for (; cur != info->table + v; cur = cur->next) {
		if (!info->hash_cmp(cur, key)) {
			continue;
		} else {
			return cur;
		}
	}
#endif
	static struct list_head *cur = NULL;
	
	if (cur == NULL || !info->hash_cmp(cur, key)) {
		cur = info->table + v;
	}

	list_for_each_continue(cur, info->table + v) {
		if (!info->hash_cmp(cur, key)) {
			continue;
		} else {
			return cur;
		}
	}

	return NULL;	
}

static void hash_push(struct hashtable *info, struct list_head *node)
{
	assert(info != NULL);
	assert(node != NULL);

	val_t v = info->hash(node) % info->nmemb;

#ifdef  DEBUG
{
	printf("tab[%lu]\n", v);
}
#endif
	list_add_tail(info->table + v, node);
}

static void hash_for_each(const struct hashtable *info,
			void (*todo)(struct list_head *node))
{
	assert(info != NULL);

	int i;
	for (i = 0; i < info->nmemb; ++i) {
		struct list_head *cur = info->table[i].next;
#ifdef  DEBUG
		printf("table[%d]:", i);
#endif
	       for (; cur != info->table + i; cur = cur->next) {
	       		todo(cur);
	       }
#ifdef DEBUG
       		printf("\n");	
#endif       
	}
}

static int hash_is_empty(const struct hashtable *info)
{
	int i;
	for (i = 0; i < info->nmemb; ++i) {
		if (list_is_empty(info->table + i)) {
			continue;
		} else {
			return 0;
		} 
	}

	return 1;
}

static int hash_is_full(const struct hashtable *info)
{
	int i;
	for (i = 0; i < info->nmemb; ++i) {
		if (!list_is_empty(info->table + i)) {
			continue;
		} else {
			return 1;
		} 
	}

	return 0;
}



int hashtable_init(struct hashtable *info, size_t nmemb,
		val_t (*hash)(const struct list_head *node),
          int (*hash_cmp)(const struct list_head *a, const struct list_head *b))
{
	assert(info != NULL);
	assert(hash != NULL);
	assert(hash_cmp != NULL);

	info->table = (struct list_head *)
			malloc(sizeof(struct list_head) * nmemb);
	if (info->table == NULL) {
		perror("malloc");
		goto err_malloc;
	}

	/*初始化*/
	int i;
	for (i = 0; i < nmemb; ++i) {
		list_head_init(info->table + i);
	}

	info->nmemb = nmemb;
	info->hash = hash;
	info->hash_cmp = hash_cmp;
	info->push = hash_push;
	//info->del = hash_del;
	info->find = hash_find;
	info->is_empty = hash_is_empty;
	info->is_full = hash_is_full;
	info->for_each = hash_for_each;

	return 0;
err_malloc:
	return -1;
}

int hashtable_destroy(struct hashtable *info)
{
	//TODO
	int i;
	for (i = 0; i < info->nmemb; ++i) {
		while (!list_is_empty(info->table + i)) {
			list_del_init(info->table[i].next);
		}
	}

	free(info->table);

	return 0;
}
