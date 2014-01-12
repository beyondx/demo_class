/*
--------------------
project : address_list
author  : tangdong
date    : 2013.4.6
version : v1.0.0
--------------------
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "address_list.h"

void help_info()
{
	printf("Through the address book, you can keep the communication and contact and acquaintances!\n");
	printf("You can export contact information.\n");
	printf("you can add, delete, modify contact information\n");
}

void print_linkman(struct list_head *node)
{
	struct link_man *pdata = NULL;
	pdata = list_entry(node, struct link_man, list);
	printf("%s %s\n",pdata->name,pdata->tel);
	printf("------------------\n");
}

int main(void)
{
	int i, j;
	node_init(&address_list);
	char *menu[7] = {"1.新增联系人","2.更新联系人","3.查找联系人","4.查看","5.导出联系人","9.帮助","0.退出"};


	printf("----------------欢迎使用本通讯录-------------\n");
	for(;;){
		for( j = 0 ; j < 7 ; ++j){
			printf("%s\n", menu[j]);
		}
		printf("[0-5,9]:");
		scanf("%d",&i);
		printf("\n");
		switch(i){
			case 1 :
				add_link_man();
				break;
			case 2 :
				display_link_man(&address_list,print_linkman);
				update_link_man(&address_list);
				break;
			case 3 :
				find_link_man(&address_list);
				break;
			case 4 :
				display_link_man(&address_list,print_linkman);
				break;
			case 5 :
				if(save(&address_list) < 0){
					printf("save failed!\n");
				}else
					printf("save succeed!\n");
				break;
			case 0 :
				list_destory(&address_list);
				exit(-1);
				break;
			case 9 :
				help_info();
				break;
			default :
				break;
		}

	}
	return 0;
}
