#include <stdio.h>
#include <stdlib.h>

typedef  int  elem_t;

struct node_info  {
	elem_t  data;
	struct node_info *next;	
};


/*面向对象的抽象编程*/

/* 该对象应该具备的方法*/
/*1. 构造： 空间的申请和初始化*/
struct node_info *slist_init(void)
{
	struct node_info *phead = (struct node_info *)malloc(sizeof(*phead));
	#if 0
	short *a;
	printf("sizeof(a) = %lu, sizeof(*a) = %lu\n", sizeof(a), sizeof(*a));
	char b[10] = "hello";
	printf("sizeof(b) = %lu, sizeof(b[0]) = %lu, sizeof(*b) = %lu\n", sizeof(b), sizeof(b[0]), sizeof(*b));
	#endif
	
	if (phead == NULL) {
		perror("malloc failed");
		goto err_malloc;
	}
	
	phead->data = 0;
	phead->next = phead;

	return phead;

err_malloc:
	return NULL;
}
/*2. 添加， 删 ， 查*/
//头插 头结点后面插入
int slist_add(struct node_info *phead, elem_t data)
{
	struct  node_info  *new_node = (struct node_info *)
		malloc(sizeof(*new_node));
	
	if (new_node == NULL) {
		perror("malloc");
		goto err_malloc;		
	}

	new_node->data = data;
	new_node->next = phead->next;
	phead->next = new_node;

err_malloc:
	return -1;
}

//尾插：尾接头后面插入
int slist_add_tail(struct node_info *phead, elem_t data)
{
	struct  node_info  *new_node = (struct node_info *)
		malloc(sizeof(*new_node));
	
	if (new_node == NULL) {
		perror("malloc");
		goto err_malloc;		
	}

	new_node->data = data;
	
	struct node_info *tail = phead->next;
	while (tail->next != phead) {
		tail = tail->next;
	}	
	
	new_node->next = tail->next;
	tail->next = new_node;

err_malloc:
	return -1;
}

//删除
int slist_del(struct node_info *phead, elem_t data)
{
	struct node_info *cur = phead->next;
	struct node_info *Prev = phead;
	struct node_info *Next = NULL;

	for (; cur != phead; cur = Next) {
		Next = cur->next;
		if (cur->data == data) {
			Prev->next = Next;
			cur->next = cur;
			free(cur);
			return 0;
		}
		Prev = cur;
	}

	return -1;
}

/*3. 遍历*/
void slist_for_each(const struct node_info *phead)
{
	const struct node_info *cur = phead->next;
	for (; cur != phead; cur = cur->next) {
		printf("%d ", cur->data);
	}
}

void __slist_for_each_invert(const struct node_info *phead, 
				struct node_info *node)
{
	if (node == phead) {
		return ;
	} else {
		__slist_for_each_invert(phead, node->next);
		printf("%d ", node->data);
	}
}

void  slist_for_each_invert(const struct node_info *phead)
{
	__slist_for_each_invert(phead, phead->next);
}

void slist_invert(struct node_info *phead)
{
	struct node_info *Prev = phead;
	struct node_info *cur =  phead->next;
	struct node_info *Next = NULL;

	for (; cur != phead; cur = Next) {
		Next = cur->next;		
		cur->next = Prev;
		Prev = cur;
	}
	//头结点	
	phead->next = Prev;
}


/*4. 析构：空间的销毁或者资源的回收*/

int slist_destroy(struct node_info *phead)
{
	struct node_info *cur = phead->next;
	struct node_info *Next = NULL;

	for (; cur  != phead; cur = Next)
	{	
		Next = cur->next;
		cur->next = cur;
		free(cur);
	}

	free(cur);
	return 0;
}

int main()
{
	struct node_info *phead = slist_init();
	
	int i;
	for (i = 0; i < 10; ++i) {
		slist_add_tail(phead, i);
	}
	
	if (slist_del(phead, 5) < 0) {
		fprintf(stderr, "slist_del failed\n");
		exit(1);
	}
	
	//slist_for_each_invert(phead);
	slist_invert(phead);
	slist_for_each(phead);
	printf("\n");
//	slist_destroy(phead);
	return 0;
}
