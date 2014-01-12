#pragma once

#include "stack.h"

typedef  int num_t;

enum FLAG_T {
	OPR,
	NUM,	
};

struct calc_info {
	struct stack_info *num;
	struct stack_info *opr;
	const char *cur;
	enum FLAG_T flag; 
	num_t (*calc)(struct calc_info *info, const char *expr);
};

int calc_init(struct calc_info *info);
int calc_destroy(struct calc_info *info);

#define   NUM_PUSH(n)	\
	info->num->push(info->num, &(n), sizeof(n))
#define   NUM_TOP(n)	\
	info->num->top(info->num, &(n), sizeof(n))
#define   NUM_POP(n)	\
	info->num->pop(info->num, &(n), sizeof(n))
#define   NUM_IS_EMPTY	\
	info->num->is_empty(info->num)

#define   OPR_PUSH(c)	\
	info->opr->push(info->opr, &(c), sizeof(char))
#define   OPR_TOP(c)	\
	info->opr->top(info->opr, &(c), sizeof(char))
#define   OPR_POP(c)	\
	info->opr->pop(info->opr, &(c), sizeof(char))
#define   OPR_IS_EMPTY	\
	info->opr->is_empty(info->opr)

