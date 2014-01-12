#pragma once

//定义一个内核链表的节点结构
struct list_head {
	struct list_head *next; 
	struct list_head *prev; 
}; 


//这两个宏对头节点进行初始化，(有头循环)
#define LIST_HEAD_INIT(head) {&head, &head}

#define LIST_HEAD(head) \
	struct list_head (head) = LIST_HEAD_INIT(head)


//内联函数，对节点进行初始化

static inline void init_list_head(struct list_head *node)
{
	node->next = node; 
	node->prev = node; 
} 


//增加结点，参数进行抽象，无需考虑头插还是尾插，由调用函数进行参数位置调整
static inline void __list_add(struct list_head *node, struct list_head *Next, struct list_head *Prev)
{
	node->next = Next; 
	node->prev = Prev; 

	Next->prev = node; 
	Prev->next = node; 
}

//头插
static inline void list_add(struct list_head *head, struct list_head *node)
{
	__list_add(node, head->next, head); 
}

//尾插
static inline void list_add_tail(struct list_head *head, struct list_head *node)
{
	__list_add(node, head, head->prev); 
}


//删除节点
static inline void list_del(struct list_head *node)
{
	node->next->prev = node->prev; 
	node->prev->next = node->next; 
}

//删除并初始化节点
static inline void list_del_init(struct list_head *node)
{
	node->next->prev = node->prev; 
	node->prev->next = node->next; 

	init_list_head(node); 
}

//判断内核链表是否为空
static inline int list_is_empty(struct list_head *head)
{
	return (head->next == head)  && (head->prev == head); 
}


//正常遍历(从头)
#define list_for_each(cur, head) \
	for((cur) = (head)->next; (cur) != (head); (cur) = (cur)->next)

//从cur节点开始进行遍历
#define list_for_each_from(cur, head) \
	for(; (cur) != (head); (cur) = (cur)->next)

//从cur的下一个节点进行遍历
#define list_for_each_continue(cur, head) \
	for((cur) = (cur)->next; (cur) != (head); (cur) = (cur)->next)

//反方向从头遍历
#define list_for_each_reverse(cur, head) \
	for((cur) = (head)->prev; (cur) != (head); (cur) = (cur)->next)

//安全遍历(从头)
#define list_for_each_safe(cur, temp, head) \
	for((cur) = (head)->next; ((cur) != (head)) &&((temp) = (cur)->next); (cur) = temp)

//从cur节点开始安全遍历
#define list_for_each_safe_from(cur, temp, head) \
	for(; ((cur) != (head)) && ((temp) = (cur)->next); (cur) = temp)

//从cur的下一个节点开始安全遍历
#define list_for_each_safe_continue(cur, temp, head) \
	for(; ((cur) != (cur)->next) &&((temp) = (cur)->next); (cur) = temp)


//小指针转换为大指针
//让编译器假设数据部分的起始地址是0地址，然后获得member的地址-0 = 偏移量；
/*
 *
 *				+--------+
 *				|        |
 *				| MEMBER |
 *	   ptr----->|--------|  -----addr == &((TYPE *)(0)->MEMBER)
 *				|        |
 *				|        |
 *				|        |
 *				|        |
 *				|        |
 *     PTR----->+--------+  -----addr == 0
 *
 *   ptr是知道的，因为内核链表的head节点知道，
 *   PTR = ptr-offsetof
 *
 */
#define offsetof(TYPE, MEMBER) \
	((size_t)&(((TYPE *)0)->MEMBER))

//获得data部分的起始地址，通过上述的offseof宏进行小指针转大指针
#define container_of(ptr, TYPE, MEMBER) \
	((TYPE *)((char *)ptr - offsetof(TYPE, MEMBER)))

