#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"


struct data_info {
	char *name;
	int age;
	struct list_head list;
	char *des;
	size_t qq;
};


int main()
{
	struct data_info s[] = {
		{"张飞", 18},
		{"关羽", 19},
		{"刘备", 20},
		{"赵云", 21},
		{"诸葛亮", 22},
		{"曹操", 23},
	};


	LIST_HEAD_INIT(head);

	int i;
	for (i = 0; i < sizeof s /sizeof s[0]; ++i) {
		list_add_tail(&head, &s[i].list);
	}
	list_del_init(&s[2].list);

	struct list_head *cur = NULL;
	list_for_each(&head, cur) {
		struct data_info *pa = list_entry(cur, struct data_info, list);
		printf("%s:%d\n", pa->name, pa->age);
	}

	struct data_info *pa = NULL;
	list_for_each_entry(pa, &head, list) {
		printf("#%s:%d\n", pa->name, pa->age);
	}


	return 0;
}



