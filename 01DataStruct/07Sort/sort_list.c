#include <stdio.h>
#include "list.h"

struct data_info {
	int data;
	struct list_head list;
};

void swap(struct list_head *pa,  struct list_head *pb)
{
	struct  list_head node = { &node, &node};
	
	__list_add(&node, pa, pa->next);
	list_del(pa);
	__list_add(pa, pb, pb->next);
	list_del(pb);
	__list_add(pb, &node, node.next);
	list_del(&node);

}

int cmp_list(const struct list_head *pa, const struct list_head *pb)
{
	struct data_info *pda = list_entry(pa, struct data_info, list);
	struct data_info *pdb = list_entry(pb, struct data_info, list);

	return pda->data - pdb->data;	
}

void bubble_sort(struct list_head *head, int (*cmp)(const struct list_head *a, const struct list_head *b))
{
	struct list_head *end = NULL;
	struct list_head *start = NULL;

	for (end = head->prev; end != head; end = end->prev) {
		for (start = head->next; 
				start != head && start != end; 
					start = start->next) {
			if (cmp(start, start->next) > 0) {
				swap(start, start->next);
				//归位，后面会默认更新
				start = start->prev;
				if (start == end) {
					//归位
					end = end->next;
				}
			}
		}
	}
}

void select_sort(struct list_head *head, 
			int (*cmp)(const struct list_head *, 
				const struct list_head *))
{
	struct  list_head *i, *j, *min;

	for (i = head->next; i != head->prev;  i = i->next) {
		min = i;
		for (j = i->next; j != head; j = j->next) {
			if (cmp(j, min) < 0) {
				min = j;
			}
		}
		if (min != i) {
			swap(min, i);
			//i 归位
			i = min;
		}
	}
}


void insert_sort(struct list_head *head, 
			int (*cmp)(const struct list_head *, 
				const struct list_head *))
{
	struct list_head *i, *j, *Next;

	for (i = head->next->next; i != head; i = Next) {
		j = i->prev;
		Next = i->next;
		if (cmp(i, j) >= 0) {
			continue;
		}
		list_del(i);
		for (; j != head; j = j->prev) {
			if (cmp(j, i) <= 0) {
				__list_add(i, j, j->next);
				break;
			}
		}
		if (j == head) {
			__list_add(i, j, j->next);
		}
	}
}

int main()
{
	struct data_info s[] = {
		{3}, {2}, {7}, {7}, {4}, {5}, {1}, {6},
	};
	
	LIST_HEAD_INIT(head);
	int i;
	for (i = 0; i < sizeof s / sizeof s[0]; ++i) {
		list_add_tail(&head, &s[i].list);
	}
	
	//swap(&s[1].list, &s[5].list);
	//bubble_sort(&head, cmp_list);
	//select_sort(&head, cmp_list);
	insert_sort(&head, cmp_list);
	struct data_info *pa = NULL;
	list_for_each_entry(pa, &head, list) {
		printf("%d ", pa->data);
	}
	printf("\n");
}
