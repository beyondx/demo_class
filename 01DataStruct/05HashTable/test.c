#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hashtable.h"
//gd 
struct data_info {
	char *name;
	int age;
	struct list_head list;
};

void print_node(struct list_head *node)
{
	struct data_info *pa = list_entry(node, struct data_info, list);
	printf("%s[%d]:", pa->name, pa->age);
}


val_t hash_xx(const struct  list_head *node)
{
	struct data_info *pa = list_entry(node, struct data_info, list);
	char *pname = pa->name;
	size_t val = 0;
	
	for (; *pname != '\0'; ++pname) {
		val += *pname;
	}
#ifdef  DEBUG
{
	printf("key:%s --> val:%lu-->", pa->name, val);
}
#endif
	return val;
}

int   hashcmp_xx(const struct list_head *a, const struct list_head *b)
{
	
	struct data_info *pa = list_entry(a, struct data_info, list);
	struct data_info *pb = list_entry(b, struct data_info, list);

	return  !strcmp(pa->name, pb->name);

}


int main()
{
	struct data_info s[] = {
		{"韦小宝", 18},
		{"陈近南", 19},
		{"韦小宝", 20},
		{"双儿", 20},
		{"鳌拜", 21},
		{"韦小宝", 22},
		{"康熙", 22},
		{"吴三桂", 23},
		{"韦小宝", 24},
	};

	struct hashtable *ht = (struct hashtable *)malloc(sizeof(*ht));
	assert(ht != NULL);

	hashtable_init(ht, 2, hash_xx, hashcmp_xx);

	int i;
	for (i = 0; i < sizeof s / sizeof s[0]; ++i) {
		ht->push(ht, &s[i].list);
	}
	printf("push-done------\n");
	ht->for_each(ht, print_node);

	printf("\nhash_find-----\n");
	struct list_head *cur = NULL;
	cur = ht->find(ht, &s[0].list);
	cur = ht->find(ht, &s[0].list);
	cur = ht->find(ht, &s[0].list);
	cur = ht->find(ht, &s[4].list);
	assert(cur != NULL);
	struct data_info *pdata =  list_entry(cur, struct data_info, list);
	printf("found:%s:%d\n", pdata->name, pdata->age);
	

	hashtable_destroy(ht);
	free(ht);

	return 0;
}
