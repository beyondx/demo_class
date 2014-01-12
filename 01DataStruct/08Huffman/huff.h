#pragma once

#include "list.h"
#define  LEAF_NUM	256 //0~255 ASCII表中的有效字符
#define  UNLEAF_NUM	(LEAF_NUM - 1) //中间结点是叶子结点-1
#define  ALL_NUM	(LEAF_NUM  + UNLEAF_NUM)
				// 0 ~ 510
#define  UNUSED		ALL_NUM // 511

typedef   unsigned short  uint16_t;
typedef   uint16_t 	elem_t;

struct huff_tree {
	uint16_t parent;
	uint16_t lchild;
	uint16_t rchild;
};


struct huff_node {
	struct list_head list;
	struct huff_tree tree;
	size_t weight; //频率次数
	unsigned short code; 
	unsigned char  code_len;
};


// table['a'] = 
#define   HUFF_MAGIC 	0x88888888
struct huff_head {
	unsigned int magic; //魔术字用来验证
	uint16_t type; // zip:0x11 rar:0x22 hz:0x33
	uint16_t root;
	size_t file_size;
	struct  huff_node table[0];
};

struct huff_info {
	struct  huff_node table[ALL_NUM]; //编码表
	FILE *pfsrc;
	FILE *pfdst;
	struct list_head head;
	uint16_t root;
	size_t file_size;
	int (*compress)(struct huff_info *info, const char *src, const char *zip);
	int (*decompress)(struct huff_info *info, const char *zip, const char *orig);
};

int  huff_init(struct huff_info *info);
int  huff_destroy(struct huff_info *info);

#define  WEIGHT(i)	(info->table[i].weight)
#define  CODE(i)	(info->table[i].code)
#define  CODE_LEN(i)	(info->table[i].code_len)
#define  LIST(i)	(info->table[i].list)
#define  TREE(i)	(info->table[i].tree)
#define  PARENT(i)	(info->table[i].tree.parent)
#define  LCHILD(i)	(info->table[i].tree.lchild)
#define  RCHILD(i)	(info->table[i].tree.rchild)

static inline void hf_tree_node_init(struct huff_tree *node)
{
	node->parent = UNUSED;
	node->lchild = UNUSED;
	node->rchild = UNUSED;
}

#define   PERROR(str)	\
	do {	\
		perror(str); \
		abort(); } while(0)

#define  FPRINTF(out, fmt, args...)  fprintf(out, fmt, ##args)

#define  DBG_PRINTF(fmt, args...)  \
        FPRINTF(stdout, __FILE__":%s:%d:[Debug]:"fmt, __FUNCTION__, __LINE__, ##args)

#define  ERR_PRINTF(fmt, args...)  \
        FPRINTF(stderr, __FILE__":%s:%d:[Error]:"fmt, __FUNCTION__, __LINE__, ##args)

#define  WARN_PRINTF(fmt, args...)  \
        FPRINTF(stderr, __FILE__":%s:%d:[Warning]:"fmt, __FUNCTION__, __LINE__, ##args)
