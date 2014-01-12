#pragma once

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};


#define   LIST_HEAD_INIT(head)	\
		struct list_head (head) = {&(head), &(head)}
/*内联函数： 函数调用时，直接把函数体中的代码拷贝到调用位置
	特点： 执行效率高，不发生传递和返回
  普通函数： 发生函数调用时，会在内存中查找以该函数名，命名的区域然后发生传递执行和返回的过程
内联函数的规则：
	1. 适用于函数提比较简洁，逻辑比较简单的函数
	2. 不能是递归函数
	3. 不能使用函数指针指向该函数
	4. 不支持除if以外的逻辑控制结构
	5. inline 一般会配合static，控制作用域，防止函数名冲突
*/
static inline void list_head_init(struct list_head *node)
{
	node->next = node;
	node->prev = node;
}


static inline void __list_add(struct list_head *node, 
			struct list_head *Prev, struct list_head *Next)
{
	node->next = Next;
	node->prev = Prev;
	Next->prev = node;
	Prev->next = node;
}

static inline void list_add(struct list_head *head, struct list_head *node)
{
	__list_add(node, head, head->next);

#if 0
	node->next = head->next;
	node->prev = head;
	head->next = node;
	node->next->prev = node;
#endif
}


static inline void list_add_tail(struct list_head *head, struct list_head *node)
{
	__list_add(node, head->prev, head);
}

static inline int list_is_empty(const struct list_head *head)
{
	return  head->next == head && head->prev == head;
}


static inline void list_del(struct list_head *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

static inline void list_del_init(struct list_head *node)
{
	list_del(node);
	node->next = node;
	node->prev = node;
	//free(node);
}


#define  list_for_each(head, cur) \
	for (cur = (head)->next; cur != (head); cur = (cur)->next)

#define  list_for_each_safe(head, cur, Next) \
	for (cur = (head)->next; Next = (cur)->next && cur != (head);\
		cur = (Next))

#define  list_for_each_reverse(head, cur)	\
	for (cur = (head)->prev; cur != (head); cur = (cur)->prev)


/*
offset_of: 求小结构体与大结构之间的偏移量
@type:  大结构体的类型
@member: 小结构体在大结构体中的成员名
*/
#define   offset_of(type, member)	\
		((size_t)&((type *)0)->member)
/*
container_of: 小指针转大指针
@ptr : 小指针
@type: 大结构体的类型
@member: 小结构体在大结构体中的成员名
*/
#define   container_of(ptr, type, member)	\
		((type *)((char *)ptr - offset_of(type, member)))

#define   list_entry(ptr, type, member)	\
		container_of(ptr, type, member)
/*
list_for_each_entry: 遍历大结构体
@pos:  大结构体类型的指针
*/
#define  list_for_each_entry(pos, head, member)	\
	for (pos = list_entry((head)->next, typeof(*pos), member);\
		&(pos)->member != (head); \
		pos = list_entry((pos)->member.next, typeof(*pos), member))

#define  list_for_each_from(cur, head) \
	for (; cur != (head); cur = (cur)->next)

#define  list_for_each_continue(cur, head) \
	for (cur = (cur)->next; cur != (head); cur = (cur)->next)
