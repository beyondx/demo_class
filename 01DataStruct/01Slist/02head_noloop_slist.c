#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* 有头非循环*/
typedef  int elem_t;

struct node_info {
	elem_t data;
	struct node_info *next;
};

/*抽象: 操作对象 + 操作方法*/
struct slist_info {
	struct node_info head;
	void (*add)(struct slist_info *info, struct node_info *node);
	void (*add_tail)(struct slist_info *info, struct node_info *node);
	struct node_info *(*search)(struct slist_info *info, 
			elem_t key);
	void (*del)(struct slist_info *info, struct node_info *key)	;
	void(*for_each)(struct slist_info *info);
};

int slist_init(struct slist_info *info);
int slist_destroy(struct slist_info *info);


static void slist_add(struct slist_info *info, struct node_info *node)
{
	assert(info != NULL);
	assert(node != NULL);
	
	node->next = info->head.next;
	info->head.next = node;
}

static void slist_for_each(struct slist_info *info)
{
	struct node_info *cur = info->head.next;
	
	for (; cur != NULL; cur = cur->next) {
		printf("%d ", cur->data);
	}
}

static void slist_add_tail(struct slist_info *info, struct node_info *node)
{
	assert(info != NULL);
	assert(node != NULL);
	
	struct node_info *cur = info->head.next;
	
	while (cur != NULL && cur->next != NULL) {
		cur = cur->next;
	}
	
	node->next = NULL;
	if (cur != NULL) {
		cur->next = node;
	} else {
		info->head.next = node;
	}
}




struct node_info *slist_search(struct slist_info *info, 
			elem_t key)
{
	assert(info != NULL);
	//1. 检查是否是空链表
	if (info->head.next == NULL) {
		fprintf(stderr, "slist is empty\n");
		return NULL;
	}
	//2. 非空
	struct node_info *cur = info->head.next;

	for (; cur != NULL; cur = cur->next) {
		if (cur->data == key) {
			return cur;
		}
	}

	return NULL;	
}

#define    slist_is_empty(slist)	\
		((slist)->head.next == NULL)

void slist_del(struct slist_info *info, struct node_info *key)
{
	assert(info != NULL);
	assert(key != NULL);

	//1. 空链表的判断
	if (slist_is_empty(info)) {
		fprintf(stderr, "slist is empty\n");
		return ;
	}
	//2. 第一个有效结点
	if (info->head.next == key) {
		info->head.next = key->next;
		key->next = key;
		//free(key)
		return ;
	}
	//3. 其他结点
	struct node_info *cur = info->head.next;

	for (; cur != NULL; cur = cur->next) {
		if (cur->next != key) {
			continue;
		}
		cur->next = key->next;
		key->next = NULL;
		return ;
	}
	//4. 未找到key
	return ;
}

int slist_destroy(struct slist_info *info)
{
	//TODO
	
	return 0;
}

int slist_init(struct slist_info *info)
{
	info->head.data = 0;
	info->head.next = NULL;

	info->add = slist_add;
	info->add_tail = slist_add_tail;
	info->search = slist_search;
	info->del = slist_del;
	info->for_each = slist_for_each;
	
	return 0;
}

int main()
{
	struct node_info s[] = {
		{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8},
	};

	struct slist_info *slist = (struct slist_info *)malloc(sizeof(*slist));
	
	if (slist == NULL) {
		perror("malloc");
		goto err_malloc;
	}

	slist_init(slist);	


	int i;
	for (i = 0; i < sizeof(s) / sizeof(s[0]); ++i) {
		slist->add_tail(slist, s + i);
	}
	
	struct node_info *k = slist->search(slist, 5);
	if (k != NULL) {
		printf("found:%d\n", k->data);
	} else {
		printf("not found\n");
	}
	slist->del(slist, k);
	
	slist->for_each(slist);
	printf("\n");
	free(slist);

	return 0;

err_malloc:
	return -1;
}
