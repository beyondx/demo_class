#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "calc.h"

/* 1  + 2 * 3  - 4 */

static int num_handle(struct calc_info *info)
{
	num_t  n =  *info->cur - '0';
	num_t  prev_n = 0;
	if (info->flag == OPR) {
		info->flag = NUM;
	} else {
		NUM_POP(prev_n);
		n += prev_n * 10;
	}
	
	//info->num->push(info->num, &n, sizeof(n));
	NUM_PUSH(n);

	return 0;
}

static int  opr2level(char c)
{
	int level = 0;

	switch (c) {
		case '*':
		case '/':
		case '%':
			level += 2;
			break;
		case '+':
		case '-':
			level += 1;
			break;
		case '(':
			break;
		default:
			fprintf(stderr, "暂不支持的运算符:'%c'\n", c);
			break;
	}

	return level;
}
/*出栈
 1. 数字栈取两次
 2. 运算符中取一次
 3. 运算
 4. 再把结果写入数字栈
 */
static int do_stack(struct calc_info *info)
{
	assert(info != NULL);
	
	num_t a, b, ret;
	char opr;
	
	if (NUM_POP(b) < 0) {
		fprintf(stderr, "num_pop failed\n");
		goto err_num_pop;
	}
	
	if (NUM_POP(a) < 0) {
		fprintf(stderr, "num_pop failed\n");
		goto err_num_pop;
	}

	if (OPR_POP(opr) < 0) {
		fprintf(stderr, "opr_pop failed\n");
		goto err_opr_pop;
	}

	switch (opr) {
		case '+':
			ret = a + b;
			break;
		case '-':
			ret = a - b;
			break;
		case '*':
			ret = a * b;
			break;
		case '/':
			if (b == 0) {
				fprintf(stderr, "不能除0\n");
				goto err_divd_zero;
			}
			ret = a / b;
			break;
		case '%':
			ret = a % b;
			break;
		default:
			fprintf(stderr,"取出不支持的运算符:'%c'\n", opr);
			break;
	}
#ifdef  DEBUG
	printf("%d %c %d = %d\n", a, opr, b, ret);
#endif

	NUM_PUSH(ret);

	return 0;

err_num_pop:
	return -1;
err_opr_pop:
	return -2;
err_divd_zero:
	return -3;
}



static int opr_handle(struct calc_info *info)
{
	assert(info != NULL);
	
	char c = *info->cur;

	if (info->flag == NUM) {
		info->flag = OPR;
	} else {
		switch (c) {
			case '+':
				goto out;
				break;
			case '-':
				//FIXME 负号的处理
				{
				  num_t zero = 0;
				  NUM_PUSH(zero);
				  OPR_PUSH(c);
				  goto out;	
				}
				break;
			default: 
				fprintf(stderr, "免费版不支持多个运算符\n");
				break;
		}
	}

	//1. 入栈的两种情况： a) 空栈  b）运算符优先级高于栈顶运算符
	
	if (OPR_IS_EMPTY) {
		OPR_PUSH(c);
	} else {
		char top_c;
		while (1) {
			//a 空栈
			if (OPR_TOP(top_c) < 0) {
				OPR_PUSH(c);
				break;
			}
			//b 优先级高于栈顶
			if (opr2level(c) > opr2level(top_c)) {
				OPR_PUSH(c);
				break;
			} else {
				if (do_stack(info) < 0) {
					fprintf(stderr,"do_stack failed\n");
					goto err_do_stack;
				}
			}
		}
	}
out:	
	return 0;
err_do_stack:
	return -1;
}

static int bracket_handle(struct calc_info *info)
{
	assert(info != NULL);
	
	char opr = *info->cur;
	switch (opr) {
		case '(':
			OPR_PUSH(opr);
			break;	
		case ')':
			{
				char top_c;
				while (1) {
					if (OPR_TOP(top_c) < 0) {
						fprintf(stderr, "Error:缺少左括号\n");
						goto err_miss_left_bracket;
					}
					if (top_c == '(') {
						OPR_POP(top_c);						       goto out;
					} else {
						do_stack(info);
					}
				}
			}
		default:
			break;
	}

out:
	return 0;

err_miss_left_bracket:
	return -1;
}

static num_t calc_main(struct calc_info *info, const char *expr)
{
	assert(info != NULL);
	assert(expr != NULL);

	info->cur = expr;
	int iret = 0;

	for (; *info->cur != '\0'; info->cur++) {
		switch (*info->cur) {
			case '0' ... '9':
				if ((iret = num_handle(info)) < 0) {
					fprintf(stderr, "num_handle failed\n");
					return iret;
				}
				break;
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
				if ((iret = opr_handle(info)) < 0) {
			fprintf(stderr, "opr_handle failed\n");
			return iret;
	}	
				break;
			case ' ':
			case '\n':
			case '\t':
				break;
			case '(':
			case ')':
				if ((iret = bracket_handle(info)) < 0) {
			fprintf(stderr, "bracket_handle failed\n");
			return iret;
}
				break;
			default:
				fprintf(stderr, "unkown char '%c'\n", *info->cur);
				goto err_unkown_char;	
		}
	}

	num_t ret = 0;
	while (!OPR_IS_EMPTY) {
		char top_c = 0;
		OPR_TOP(top_c);
		if (top_c == '(') {
			fprintf(stderr, "Error: 缺少对应的右括号\n");
			goto err_miss_right_bracket;
		}
		if (do_stack(info) < 0) {
			fprintf(stderr, "do_stack failed\n");
			goto  err_do_stack;
		}
	}

	NUM_POP(ret);

	return ret;
err_unkown_char:
	return -1;
err_do_stack:
	return -2;
err_miss_right_bracket:
	return -3;
}



int calc_init(struct calc_info *info)
{
	assert(info != NULL);

	info->num = (struct stack_info *)malloc(sizeof(struct stack_info));
	info->opr = (struct stack_info *)malloc(sizeof(struct stack_info));
	
	if ((info->num == NULL) || (info->opr == NULL)) {
		perror("malloc");
		goto err_malloc;
	}

	stack_init(info->num);
	stack_init(info->opr);

	info->cur = NULL;
	info->flag = OPR;

	info->calc = calc_main;

	return 0;
err_malloc:
	return -1;
}

int calc_destroy(struct calc_info *info)
{
	stack_destroy(info->num);
	stack_destroy(info->opr);
	free(info->num);
	free(info->opr);

	return 0;
}
