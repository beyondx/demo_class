#include <stdio.h>


//再宏的填充语句中，使用'#',把参数转化为字符串
#define   FOO(expr)	(#expr)
#define   FOO2(expr)	(expr)
//'##': 用来区分普通字符和参数,并将结果拼接在一起，没有间隔符号
#define   BAR(expr, two)	(abcexpr two)
#define   BAR2(expr, two)	(abc##expr##two)

#ifndef   DBG_PRINTF
#include <time.h>
#define   DBG_PRINTF(fmt, args...)	\
	do {\
	time_t t = time(NULL);\
	fprintf(stderr, fmt"[%s:%s:%d] %s", ##args, __FILE__,  __FUNCTION__, __LINE__, ctime(&t));\
}while (0)
#endif
int main()
{
#if 0
	FOO2(1 + 2);	
	FOO(1 + 2);
	BAR(hello, 200)	
	BAR2(hello, 200)	
#endif
	DBG_PRINTF("hello %d", 1988);

	return 0;
}
