#pragma once

struct bnode_info {
	struct bnode_info *parent;
	struct bnode_info *lchild;
	struct bnode_info *rchild;	
};

struct  btree_info {
	struct bnode_info *root;
	int (*cmp)(const struct bnode_info *a,
			 const struct bnode_info *b);
	void (*push)(struct btree_info *info, 
			struct bnode_info *node);
	//(*del);
	void (*pre_order)(struct btree_info *info,
		 void (*todo)(struct bnode_info *node));
	void (*in_order)(struct btree_info *info, 
		void (*todo)(struct bnode_info *node));
	void (*post_order)(struct btree_info *info,
		 void (*todo)(struct bnode_info *node));
};

int btree_init(struct btree_info *info, 
		int (*cmp)(const struct bnode_info *a, const struct bnode_info *b));

int btree_destroy(struct btree_info *info);

static inline  void bnode_init(struct bnode_info *node)
{
	node->parent = NULL;
	node->lchild = NULL;
	node->rchild = NULL;
}

#define  offset_of(type, member)	\
	((size_t)&(((type *)0)->member))

#define  container_of(ptr, type, member) \
	((type *)((char *)ptr - offset_of(type, member)))	

#define list_entry(ptr, type, member)	\
	container_of(ptr, type, member)


