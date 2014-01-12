#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "address_list.h"
static void print_info(struct link_man *pdata)
{
	printf("姓名：%s\n",pdata->name);
//	printf("姓名拼音：%s\n",pdata->spell);
	printf("电话：%s\n",pdata->tel);
	printf("邮箱：%s\n",pdata->email);
	printf("公司：%s\n",pdata->company);
	printf("家庭住址：%s\n",pdata->address);
	printf("邮编：%s\n",pdata->post);

}

void add_link_man()
{
	char c;
	struct link_man *new = NULL;
	new = (struct link_man *)malloc(sizeof(*new));

	do{
		printf("------新增联系人------\n");
		printf("姓名：");
		scanf("%s",new->name);
		printf("姓名拼音：");
		setbuf(stdin, NULL);
		scanf("%s",new->spell);
		printf("电话：");
		scanf("%s",new->tel);
		setbuf(stdin,NULL);
		printf("email：");
		scanf("%s",new->email);
		printf("公司：");
		scanf("%s",new->company);
		printf("家庭地址：");
		scanf("%s",new->address);
		printf("邮编：");
		scanf("%s",new->post);	
		
		printf("\n");
		printf("确认信息？[Y(yes)/N(no)/B(back)]:");
		setbuf(stdin,NULL);
		scanf("%c",&c);
		if(c == 'Y' || c == 'y'){
			list_add_tail(&new->list, &address_list);
			printf("\n");
			printf("********添加成功!********\n");
			printf("\n");
		}
		else if( c == 'B' || c == 'b'){
			free(new);
			return ;
		}else
			free(new);

	}while(c == 'N' || c == 'n');
}

void update_link_man(struct list_head *head)
{
	int i;
	char c;
	char update[20];
	char update_info[20];
	printf("\n");
	printf("-------------------------\n");
	printf("[1.修改/2.删除]:");
	scanf("%d", &i);
	printf("-------------------------\n");
	if(i == 1){
		printf("输入要修改人的名字或电话：");
		scanf("%s",update);
		struct link_man *pdata = NULL;
		list_for_each_entry(pdata, head, list){
			if(strcmp(pdata->name, update) == 0 || strcmp(pdata->tel, update) == 0){
				print_info(pdata);
				do{
					printf("请输入要修改的信息：[1.name/2.spell/3.tel/4.email/5.company/6.address/7.post] :");
					scanf("%d",&i);
					switch(i){
						case 1 :
							printf("new name :");
							scanf("%s",update_info);
							memmove(pdata->name, update_info,strlen(update_info));
							break;
						case 2 :
							printf("new spell:");
							scanf("%s",update_info);
							memmove(pdata->spell, update_info,strlen(update_info));
							break;
						case 3 : 
							printf("new tel :");
							scanf("%s",update_info);
							memmove(pdata->tel, update_info,strlen(update_info));
							break;
						case 4 :
							printf("new email :");
							scanf("%s",update_info);
							memmove(pdata->name, update_info,strlen(update_info));
							break;
						case 5 :
							printf("new company :");
							scanf("%s",update_info);
							memmove(pdata->company, update_info,strlen(update_info));
							break;
						case 6 :
							printf("new address :");
							scanf("%s",update_info);
							memmove(pdata->address, update_info,strlen(update_info));
							break;
						case 7 :
							printf("new post :");
							scanf("%s",update_info);
							memmove(pdata->post, update_info,strlen(update_info));
							break;
						default : 
							break;
					}
					printf("----------------------------\n");
					printf("new record:\n");
					print_info(pdata);
					printf("确认修改或继续修改[Y/C]:");
					setbuf(stdin, NULL);
					scanf("%c",&c);
					if(c == 'Y' || c == 'y'){
						return ;
					}else{
						printf("\n");
						printf("********信息已更新！********\n");
						return ;
					}
				}while(c == 'C' || c == 'c');
			}
		}
			printf("link_man is not found!\n");
				return ;
	}
	if(i == 2){
		printf("输入要删除人的名字或电话：");
		scanf("%s",update);
		struct list_head *cur = NULL;
		struct link_man *pdata = NULL;
		list_for_each(cur, head){
			pdata = list_entry(cur, struct link_man, list);
			if(strcmp(pdata->name, update) == 0 || strcmp(pdata->tel, update) ==0){
				print_info(pdata);
				printf("删除此联系人?[Y/N]");
				setbuf(stdin, NULL);
				scanf("%c",&c);
				if(c == 'Y' || c == 'y'){
					node_del(cur);
					return ;
				}else
					return ;
			}else{
				printf("link_man is not found!\n");
					return ;
			}
		}
		
	}
}

void display_link_man(struct list_head *head, void (*todo)(struct list_head *node))
{
	assert(NULL != head && NULL != todo);

	printf("---------联系人----------\n");
	struct list_head *cur = NULL;
	list_for_each(cur, head){
		todo(cur);
	}
}

//struct list_head *find_link_man(struct list_head *head)
void find_link_man(struct list_head *head)
{
	int flag = 0;
	char find_info[20];
	printf("查找：");
	scanf("%s",find_info);
	printf("------find result-------\n");
	struct link_man *pdata = NULL;
	list_for_each_entry(pdata, head, list){
		if(strcmp(pdata->name,find_info) == 0){
			flag = 1;
			print_info(pdata);
			printf("-------------------------\n");
		}else if(strstr(pdata->tel, find_info) != NULL){
			flag = 1;
			print_info(pdata);	
			printf("-------------------------\n");
		}else if(strstr(pdata->email,find_info) != NULL){
			flag = 1;
			print_info(pdata);
			printf("-------------------------\n");
		}else if(strstr(pdata->company,find_info) != NULL){
			flag = 1;
			print_info(pdata);
			printf("-------------------------\n");
		}else if(strstr(pdata->address, find_info) != NULL){
			flag = 1;
			print_info(pdata);
			printf("-------------------------\n");
		}else if(strstr(pdata->post, find_info) != NULL){
			flag = 1;
			print_info(pdata);
			printf("-------------------------\n");
		}else
			;
	}
	if(flag == 0)
		printf("link_man is not found!\n");
	else
		return ;
}

void list_destory(struct list_head *head)
{
	while(head->next != head){
		node_del(head->next);
	}
	return ;
}
