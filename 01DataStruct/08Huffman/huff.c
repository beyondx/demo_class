#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "huff.h"

static int hf_statistic_weight(struct huff_info *info)
{
	unsigned char rbuf = 0;
	size_t n = 0;
	while ((n = fread(&rbuf, 1, 1, info->pfsrc)) != 0) {
		WEIGHT(rbuf) += 1;
	}

#ifdef  DEBUG
{
	for (n = 0; n < LEAF_NUM; ++n) {
		if (WEIGHT(n) != 0) {
			printf("'%c'=%lu\n", (unsigned char)n, WEIGHT(n));
		}
	}
}
#endif
	return 0;
}

static int list_node_cmp(const struct list_head *a, const struct list_head *b)
{
	struct huff_node *pa = list_entry(a, struct huff_node, list);
	struct huff_node *pb = list_entry(b, struct huff_node, list);
	
	return pa->weight - pb->weight;
}

void insert_sort(struct list_head *head,
                        int (*cmp)(const struct list_head *,
                                const struct list_head *));

static int hf_build_sort_list(struct huff_info *info)
{
	size_t i;
	for (i = 0; i < LEAF_NUM; ++i) {
		if (WEIGHT(i) != 0)
		list_add_tail(&info->head, &LIST(i));
	}
	
#ifdef DEBUG
{
	printf("before sort-----\n");
	struct huff_node *pa = NULL;
	
	list_for_each_entry(pa, &info->head, list) {
		size_t n = pa - info->table;
		printf("tab[%lu]:'%c':%lu\n", n, (unsigned char)n, WEIGHT(n));
	}
}
#endif
	insert_sort(&info->head, list_node_cmp);

#ifdef DEBUG
{
	printf("after sort-------\n");
	struct huff_node *pa = NULL;
	
	list_for_each_entry(pa, &info->head, list) {
		size_t n = pa - info->table;
		printf("tab[%lu]:'%c':%lu\n", n, (unsigned char)n, WEIGHT(n));
	}
}
#endif

	return 0;
}

static void list_add_with_sort(struct list_head *head, struct list_head *node)
{
	struct list_head *cur = NULL;
	for (cur = head->next; cur != head; cur = cur->next) {
		if (list_node_cmp(node, cur) > 0) {
			continue;
		} else {
			break;
		}
	}
		
	__list_add(node, cur->prev, cur);
}

static int hf_build_hufftree(struct huff_info *info)
{

	uint16_t index = LEAF_NUM; //256

	//1. 循环直到链表中只剩下一个结点
	while (info->head.next->next != &info->head) {
		//2. 选出权值最小的两个结点
		struct  list_head *a = info->head.next;
		struct  list_head *b = a->next;
		struct  huff_node *pa = list_entry(a, struct huff_node, list);
		struct  huff_node *pb = list_entry(b, struct huff_node, list);
		uint16_t pan = pa - info->table;
		uint16_t pbn = pb - info->table;
		//3. 创建一个中间结点,其权值是前两者之和
		WEIGHT(index) = pa->weight + pb->weight;
		LCHILD(index) = pan;
		RCHILD(index) = pbn;
		PARENT(pan) = index;
		PARENT(pbn) = index;
#ifdef  DEBUG
{
	printf("table[%hu](%lu)=table[%hu](%lu) + table[%hu](%lu)\n",
		index, WEIGHT(index), pan, WEIGHT(pan), pbn, WEIGHT(pbn));
}
#endif
	//4. 删除前两个结点，将中间结点添加到有序序列中
		list_del_init(a);
		list_del_init(b);

		list_add_with_sort(&info->head, &LIST(index));
		++index;
	}
	//5. 指定根结点: index 之前因为++index多加一次
	if (index != 256) {
		info->root = index - 1;
	} else {
		info->root = 256;
	}
#ifdef  DEBUG
{
	printf("info->root:%hu\n", info->root);
}
#endif
	return 0;
}

static int hf_fill_code(struct huff_info *info)
{
	uint16_t cur = UNUSED;
	uint16_t i;
	for (i = 0; i < LEAF_NUM; ++i) {
		if (WEIGHT(i) == 0) {
			continue;
		}
		cur = i;
		while (cur != info->root) {
			CODE(i) <<= 1;
			if (RCHILD(PARENT(cur)) == cur) {
				CODE(i) |= 1;
			}
			cur = PARENT(cur);
			CODE_LEN(i) += 1;
		}
#ifdef DEBUG
{
	printf("table[%hu]('%c'):%x\n", i, (unsigned char )i, CODE(i));
}
#endif	
	}


	return 0;
}


static int huff_prepare(struct huff_info *info)
{
	assert(info != NULL);
	//1. 遍历文件,统计出现的频率
	if (hf_statistic_weight(info) < 0) {
		ERR_PRINTF("hf_statistic_weight failed\n");
		goto err_statistic_weight;
	}
	//2. 构建有序序列（按照权值从小到大排列）
	if (hf_build_sort_list(info) < 0) {
		ERR_PRINTF("hf_build_sort_list failed\n");
		goto err_build_sort_list;
	}	

	//3. 构建最优二叉树
	if (hf_build_hufftree(info) < 0) {
		ERR_PRINTF("hf_build_huff_tree failed\n");
		goto err_build_huff_tree;
	}
	//4. 填充编码
	if (hf_fill_code(info) < 0) {
		ERR_PRINTF("hf_fill_code failed\n");
		goto err_fill_code;
	}	
	return 0;

err_statistic_weight:
	return -1;
err_build_sort_list:
	return -2;
err_build_huff_tree:
	return -3;
err_fill_code:
	return -4;
}

static int hf_make_huff_head(struct huff_info *info)
{
	size_t head_size = sizeof(struct huff_head) + sizeof(struct huff_node) * (info->root + 1);
	struct huff_head *phf_head = (struct huff_head *)
		malloc(head_size);

	assert(phf_head != NULL);

	phf_head->magic = HUFF_MAGIC;
	phf_head->type = 0x33; //HZ 0x33
	//将文件流置到文件结尾
	fseek(info->pfsrc, 0, SEEK_END);
	//获取原文件大小
	phf_head->file_size = ftell(info->pfsrc);
	phf_head->root = info->root;
	memcpy(phf_head->table, info->table, sizeof(struct huff_node) * (info->root + 1));
	
	fseek(info->pfdst, 0, SEEK_SET);
	fwrite(phf_head, head_size, 1, info->pfdst);
#ifdef DEBUG
	{
		printf("magic:%x\n", phf_head->magic);
		printf("type:%x\n", phf_head->type);
		printf("file_size:%lu\n", phf_head->file_size);
		printf("head_size:%lu\n", head_size);
		printf("root:%hu\n", phf_head->root);
	}
#endif	

	free(phf_head);

	return 0;
}


static int huff_compress(struct huff_info *info, const char *src, const char *dst)
{
	assert(info != NULL);
	if (src == NULL) {
		info->pfsrc = stdin;
	} else {
		info->pfsrc = fopen(src, "r");
		if (info->pfsrc == NULL) {
			ERR_PRINTF("fopen \"%s\" failed\n", src);
			PERROR("");
		}
	}
	if (dst == NULL) {
                info->pfdst = stdout;
		setbuf(stdout, NULL); //关闭标准输出的缓冲功能
        } else {
                info->pfdst = fopen(dst, "w");
                if (info->pfdst == NULL) {
                        ERR_PRINTF("fopen \"%s\" failed\n", src);
                        PERROR("");
                }
        }

	if (huff_prepare(info) < 0) {
		ERR_PRINTF("huff_prepare failed\n");
		goto err_huff_prepare;
	}
	//1.写入huff头部信息
	if (hf_make_huff_head(info) < 0) {
		ERR_PRINTF("make_huff_head failed\n");
		goto err_make_huff_head;
	}


	//2. 再次遍历文件，根据编码表写入目标文件
	rewind(info->pfsrc);

	unsigned char rbuf = 0;
	unsigned char wbuf = 0;
	uint16_t i;
	uint16_t count = 0;
	while (fread(&rbuf, 1, 1, info->pfsrc) != 0) {
		for (i = 0; i < CODE_LEN(rbuf); ++i) {
			wbuf <<= 1;
			wbuf |= (CODE(rbuf) >> i) & 1;
			++count;
			if (count == 8) {
				fwrite(&wbuf, 1, 1, info->pfdst);
#ifdef DEBUG
{
			printf("%x ", (unsigned char)wbuf);
			fflush(stdout);
}
#endif				
				wbuf = 0;
				count = 0;
			}	
		}
	}
	//将未完成wbuf部分写入目标文件
	if (count != 0) {
		wbuf <<= 8 - count;
		fwrite(&wbuf, 1, 1, info->pfdst);
#ifdef DEBUG
		printf("%x\n", wbuf);
#endif
	}


	return 0;

err_huff_prepare:
	return -1;
err_make_huff_head:
	return -2;
}

static int hf_unpack_huff_head(struct huff_info *info)
{
	struct huff_head *phf_head = (struct huff_head *)malloc(sizeof(*phf_head));
	
	assert(phf_head != NULL);
	
	fread(phf_head, sizeof(*phf_head), 1, info->pfsrc);
#ifdef  _DEBUG
{
	printf("magic:%x\n", phf_head->magic);
	printf("type:%x\n", phf_head->type);
	printf("file_size:%lu\n", phf_head->file_size);
	printf("root:%hu\n", phf_head->root);
}
#endif
	info->root = phf_head->root;
	info->file_size = phf_head->file_size;
	//size_t table_size = sizeof(struct huff_node) * (info->root + 1);
	
	//验证魔术字
	if (phf_head->magic != HUFF_MAGIC) {
		ERR_PRINTF("文件损坏,请重试\n");
		goto err_magic;
	}


	fread(info->table, sizeof(struct huff_node), info->root + 1, info->pfsrc);
#ifdef _DEBUG
	printf("head_size:%lu\n", ftell(info->pfsrc));
#endif	

	free(phf_head);
	return 0;
err_magic:
	return -1;
}


static int huff_decompress(struct huff_info *info, const char *src, const char *dst)
{
	assert(info != NULL);
	
	if (src == NULL) {
		info->pfsrc = stdin;
	} else {
		info->pfsrc = fopen(src, "r");
		if (info->pfsrc == NULL) {
			ERR_PRINTF("fopen \"%s\" failed\n", src);
			PERROR("");
		}
	}
	if (dst == NULL) {
                info->pfdst = stdout;
		setbuf(stdout, NULL); //关闭标准输出的缓冲功能
        } else {
                info->pfdst = fopen(dst, "w");
                if (info->pfdst == NULL) {
                        ERR_PRINTF("fopen \"%s\" failed\n", src);
                        PERROR("");
                }
        }

	//1. 解开压缩包的头部信息
	if (hf_unpack_huff_head(info) < 0) {
		ERR_PRINTF("hf_unpack_huff_head failed");
		goto err_unpack_huff_head;
	}

	//2.根据编码表将编码转化字符
#if 0
{
	unsigned char rbuf = 0;
	while (fread(&rbuf, 1, 1, info->pfsrc) != 0) {
		printf("%x ", rbuf);
	}
	printf("\n");
}
#endif
	
	uint16_t cur;
	uint16_t i;
	unsigned char rbuf;
	volatile size_t file_size = 0;	
	cur = info->root;
	while (fread(&rbuf, 1, 1, info->pfsrc) != 0) {
		for (i = 0; i < 8; ++i) {
			if ((rbuf >> (7 - i)) & 1) {
				cur = RCHILD(cur);
			} else {
				cur = LCHILD(cur);
			}
			//直到叶子节点为有效数据，写入目标文件
			if (cur < LEAF_NUM) {
				fwrite(&cur, 1, 1, info->pfdst);
				cur = info->root;
				if (++file_size == info->file_size) {
			goto out;
	}
			}
		}
	}
	
out:
	return 0;
err_unpack_huff_head:
	return -1;
}

int huff_init(struct huff_info *info)
{
	assert(info != NULL);
	//初始化编码表	
	int i;
	for (i = 0; i < ALL_NUM; ++i) {
		WEIGHT(i) = 0;
		CODE(i) = 0;
		CODE_LEN(i) = 0;
		list_head_init(&LIST(i));
		hf_tree_node_init(&TREE(i));
	}

	//文件指针
	info->pfsrc = NULL;
	info->pfdst = NULL;
	//链表头结点初始化
	list_head_init(&info->head);
	info->root = UNUSED;
	
	//挂接函数
	info->compress = huff_compress;
	info->decompress = huff_decompress;

	return 0;
}

int huff_destroy(struct huff_info *info)
{
	fclose(info->pfsrc);
	fclose(info->pfdst);
	return 0;
}
