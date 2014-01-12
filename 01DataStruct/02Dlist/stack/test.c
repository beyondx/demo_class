#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int main()
{
	int s[] = {1, 3, 5, 7, 9};
	
	struct stack_info *stack = (struct stack_info *)
			malloc(sizeof(*stack));

	if (stack == NULL) {
		perror("malloc");
		goto err_malloc;
	}
	
	stack_init(stack);

	int i;
	for (i = 0; i < sizeof s / sizeof s[0]; ++i) {
		stack->push(stack, s + i, sizeof s[0]);
	}

	int t;
	while (stack->pop(stack, &t, sizeof(t)) == 0) {
		printf("t = %d\n", t);
	}

	stack_destroy(stack);

	return 0;
err_malloc:
	return -1;
}
