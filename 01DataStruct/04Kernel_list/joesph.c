#include <stdio.h>
#include "list.h"

struct data_info {
	int data;
	struct list_head list;
};


// step <= nmemb
void joesph_loop(struct list_head *head, size_t start, size_t step)
{
	//遍历的起始节点
	struct list_head *cur = NULL;
	list_for_each(head, cur) {
		if (--start == 0) {
			break;
		}
	}
	cur = cur->prev; // 修正一下位置，为了后面的list_for_each_continue

	//按照指定步长开始删除节点
	size_t n = 0;
	while (head->next->next != head) {
		list_for_each_continue(cur, head) {
			++n;
			if (n % step == 0) {
#ifdef DEBUG
{
	struct data_info *pa = list_entry(cur, struct data_info, list);
	printf("del:%d\n", pa->data);
}
#endif
				list_del(cur);
			}
		}		
	
	}


	//当链表只剩下一个节点时停止循环，打印剩余的节点
	struct data_info *pa = list_entry(head->next, struct data_info, list);
	printf("The last one:%d\n", pa->data);

}


int main()
{
	struct data_info s[] = {
		{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9},
	};

	LIST_HEAD_INIT(head);
	printf("&head = %p", &head);
	int i;
	for (i = 0; i < sizeof s / sizeof s[0]; ++i) {
		list_add_tail(&head, &s[i].list);
	}	

	struct data_info *pdata = NULL;
	list_for_each_entry(pdata, &head, list) {
		printf("%d ", pdata->data);
	}
	printf("&head = %p\n", &head);

	joesph_loop(&head, 4, 4);

	return 0;
}
