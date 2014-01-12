#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "address_list.h"

int save(struct list_head *head)
{
	if(head == NULL){
		printf("-----------------------\n");
		printf("没有可导出的联系人!\n");
		printf("-----------------------\n");
	}
	if(head->next == head){
		printf("-----------------------\n");
		printf("没有可导出的联系人!\n");
		printf("-----------------------\n");
	}
	char *str = "------------------------";
	FILE *fp = NULL;

	if((fp = fopen(LIST_PATH,"w+")) == NULL){
		printf("open file failed!\n");
		return -1;
	}
	struct link_man *pdata = NULL;
	list_for_each_entry(pdata, head, list){
		fwrite(pdata->name, strlen(pdata->name), 1, fp);
		fwrite("\n", 1, 1,fp);
		fwrite(pdata->tel, strlen(pdata->tel), 1, fp);
		fwrite("\n", 1, 1,fp);
		fwrite(pdata->email, strlen(pdata->email), 1, fp);
		fwrite("\n", 1, 1,fp);
		fwrite(pdata->company, strlen(pdata->company), 1, fp);
		fwrite("\n", 1, 1,fp);
		fwrite(pdata->address, strlen(pdata->address), 1, fp);
		fwrite("\n", 1, 1,fp);
		fwrite(pdata->spell, strlen(pdata->spell), 1, fp);
		fwrite("\n", 1, 1,fp);
		fwrite(str, strlen(str), 1, fp);
	}

	fclose(fp);
	return 0;
}

#if 0
int read_linkman()
{
	FILE *fp = NULL;

	if((fp = fopen(LIST_PATH,"r+") == NULL)){
		printf("没有可导入的联系人列表\n");
		return -1;
	}

	struct link_man *new = (struct link_man *)malloc(sizeof(*new));

	




}
#endif
