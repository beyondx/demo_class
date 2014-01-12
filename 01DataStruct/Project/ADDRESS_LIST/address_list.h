#pragma once

#define LIST_PATH  "/home/tangdong/Documents/mine/address_list/link.txt"

#define MAX_NAME 20
#define MAX_TEL 15
#define MAX_EMAIL 30
#define _CO 30
#define ADDR 50
#define POST_NUM 7
#define MAX_SPELL 20

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

struct link_man {
	char name[MAX_NAME];
	char tel[MAX_TEL];
	char email[MAX_EMAIL];
	char company[_CO];
	char address[ADDR];
	char post[POST_NUM];
	char spell[MAX_SPELL];
	struct list_head list;
};

extern void add_link_man();
extern void list_destory(struct list_head *head);
extern void display_link_man(struct list_head *head, void (*todo)(struct list_head *node));
extern void find_link_man(struct list_head *head);
extern int save(struct list_head *head);
//extern int read_linkman();
extern void update_link_man(struct list_head *head);

struct list_head address_list;

static inline void __list_add(struct list_head *new_node, struct list_head *Prev, struct list_head *Next)
{
	new_node->next = Next;
	new_node->prev = Prev;

	Next->prev = new_node;
	Prev->next = new_node;
}

static inline void list_add_tail(struct list_head *node, struct list_head *head)
{
	__list_add(node, head->prev, head);
}

static inline void node_init(struct list_head *node)
{
	node->next = node;
	node->prev = node;
}

#define list_for_each(cur, head)	\
		for(cur = (head)->next ; cur != head; cur = cur->next)

#define list_for_each_continue(cur, head)	\
		for(cur = (cur)->next; cur != head; cur = (cur)->next)

#define offset_of(type, member)	\
		(size_t)(&((type *)0)->member)

#define container_of(ptr, type, member)	\
		(type *)((char *)(ptr) - offset_of(type, member))

#define list_entry(ptr, type, member)	\
		container_of(ptr, type, member)

#define list_for_each_entry(pos, head, member)	\
		for(pos = container_of((head)->next, typeof(*(pos)), member); &((pos)->member) != head;\
		pos = container_of((pos)->member.next, typeof(*(pos)), member))

static inline void node_del(struct list_head *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
	node_init(node);

	struct link_man *pdata = NULL;
	pdata = list_entry(node, struct link_man,list);
	free(pdata);
}

